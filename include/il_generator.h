#ifndef ARKOI_LANGUAGE_IL_GENERATOR_H
#define ARKOI_LANGUAGE_IL_GENERATOR_H

#include <stack>

#include "visitor.h"
#include "instruction.h"

class IRGenerator : public NodeVisitor {
public:
    IRGenerator() : _scopes(), _instructions(), _current_operand(), _temp_index(0) {}

    void visit(const ProgramNode &node) override;

    void visit(const FunctionNode &node) override;

    void visit(const TypeNode &node) override;

    void visit(const BlockNode &node) override;

    void visit(const ParameterNode &node) override;

    void visit(const NumberNode &node) override;

    void visit(const ReturnNode &node) override;

    void visit(const IdentifierNode &node) override;

    void visit(const BinaryNode &node) override;

    [[nodiscard]] const std::vector<std::unique_ptr<Instruction>> &instructions() const { return _instructions; }

private:
    std::shared_ptr<Symbol> _make_temporary();

private:
    std::vector<std::unique_ptr<Instruction>> _instructions;
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    Operand _current_operand;
    size_t _temp_index;
};

#endif //ARKOI_LANGUAGE_IL_GENERATOR_H
