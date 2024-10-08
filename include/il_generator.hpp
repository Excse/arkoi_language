#pragma once

#include "instruction.hpp"
#include "visitor.hpp"
#include "type.hpp"
#include "cfg.hpp"

class IRGenerator : NodeVisitor {
private:
    IRGenerator() = default;

public:
    [[nodiscard]] static IRGenerator generate(ProgramNode &node);

    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override {};

    void visit(IntegerNode &node) override;

    void visit(FloatingNode &node) override;

    void visit(BooleanNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    void visit(CallNode &node) override;

    void visit(IfNode &node) override;

    [[nodiscard]] auto &functions() const { return _functions; }

private:
    Operand _make_temporary(const Type &type);

    std::shared_ptr<Symbol> _make_label_symbol();

private:
    std::shared_ptr<BasicBlock> _function_end_block{};
    std::shared_ptr<Symbol> _function_end_symbol{};
    std::shared_ptr<BasicBlock> _current_block{};
    size_t _temp_index{}, _label_index{};
    Operand _current_operand{nullptr};
    std::vector<Function> _functions{};
};