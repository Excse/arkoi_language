#pragma once

#include <stack>

#include "instruction.hpp"
#include "visitor.hpp"
#include "type.hpp"

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

    [[nodiscard]] auto &instructions() const { return _instructions; }

private:
    Operand _make_temporary(const Type &type);

    std::shared_ptr<Symbol> _make_label_symbol();

private:
    std::vector<std::unique_ptr<Instruction>> _instructions{};
    std::shared_ptr<Symbol> _function_end{};
    size_t _temp_index{}, _label_index{};
    Operand _current_operand{nullptr};
};