#pragma once

#include "mid/instruction.hpp"
#include "def/type.hpp"
#include "mid/cfg.hpp"
#include "utils/visitor.hpp"

namespace arkoi::mid {

class Generator : ast::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Generator generate(ast::Program &node);

    void visit(ast::Program &node) override;

    void visit(ast::Function &node) override;

    void visit(ast::Block &node) override;

    void visit(ast::Parameter &) override {};

    void visit(ast::Integer &node) override;

    void visit(ast::Floating &node) override;

    void visit(ast::Boolean &node) override;

    void visit(ast::Return &node) override;

    void visit(ast::Identifier &node) override;

    void visit(ast::Binary &node) override;

    void visit(ast::Cast &node) override;

    void visit(ast::Call &node) override;

    void visit(ast::If &node) override;

    [[nodiscard]] auto &functions() { return _functions; }

private:
    mid::Variable _make_temporary(const Type &type);

    SharedSymbol _make_label_symbol();

private:
    std::shared_ptr<BasicBlock> _function_end_block{};
    std::shared_ptr<BasicBlock> _current_block{};
    size_t _temp_index{}, _label_index{};
    std::vector<Function> _functions{};
    SharedSymbol _function_end_symbol;
    Operand _current_operand{};
};

}