#pragma once

#include <stack>

#include "instruction.h"
#include "visitor.h"
#include "type.h"

class IRGenerator : public NodeVisitor {
public:
    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override {};

    void visit(IntegerNode &node) override;

    void visit(FloatingNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    [[nodiscard]] auto &instructions() const { return _instructions; }

private:
    std::shared_ptr<Operand> _make_temporary(const Type &type);

private:
    std::vector<std::unique_ptr<Instruction>> _instructions{};
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::shared_ptr<Operand> _current_operand{};
    size_t _temp_index{};
};