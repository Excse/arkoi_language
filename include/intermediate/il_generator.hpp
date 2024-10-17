#pragma once

#include "instruction.hpp"
#include "utils/visitor.hpp"
#include "semantic/type.hpp"
#include "intermediate/cfg.hpp"

namespace arkoi::intermediate {

class IRGenerator : ast::NodeVisitor {
private:
    IRGenerator() = default;

public:
    [[nodiscard]] static IRGenerator generate(ast::ProgramNode &node);

    void visit(ast::ProgramNode &node) override;

    void visit(ast::FunctionNode &node) override;

    void visit(ast::BlockNode &node) override;

    void visit(ast::ParameterNode &) override {};

    void visit(ast::IntegerNode &node) override;

    void visit(ast::FloatingNode &node) override;

    void visit(ast::BooleanNode &node) override;

    void visit(ast::ReturnNode &node) override;

    void visit(ast::IdentifierNode &node) override;

    void visit(ast::BinaryNode &node) override;

    void visit(ast::CastNode &node) override;

    void visit(ast::CallNode &node) override;

    void visit(ast::IfNode &node) override;

    [[nodiscard]] auto &cfgs() { return _cfgs; }

private:
    Operand _make_temporary(const type::Type &type);

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