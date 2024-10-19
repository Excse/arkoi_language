#pragma once

#include "semantic/type.hpp"
#include "utils/visitor.hpp"
#include "il/instruction.hpp"
#include "il/cfg.hpp"

namespace il {

class Generator : node::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Generator generate(node::Program &node);

    void visit(node::Program &node) override;

    void visit(node::Function &node) override;

    void visit(node::Block &node) override;

    void visit(node::Parameter &) override {};

    void visit(node::Integer &node) override;

    void visit(node::Floating &node) override;

    void visit(node::Boolean &node) override;

    void visit(node::Return &node) override;

    void visit(node::Identifier &node) override;

    void visit(node::Binary &node) override;

    void visit(node::Cast &node) override;

    void visit(node::Call &node) override;

    void visit(node::If &node) override;

    [[nodiscard]] auto &cfgs() { return _cfgs; }

private:
    Operand _make_temporary(const Type &type);

    std::shared_ptr<Symbol> _make_label_symbol();

private:
    std::shared_ptr<BasicBlock> _function_end_block{};
    std::shared_ptr<Symbol> _function_end_symbol{};
    std::shared_ptr<BasicBlock> _current_block{};
    size_t _temp_index{}, _label_index{};
    Operand _current_operand{nullptr};
    std::vector<CFG> _cfgs{};
};

}