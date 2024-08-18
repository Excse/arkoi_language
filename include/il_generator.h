#ifndef ARKOI_LANGUAGE_IL_GENERATOR_H
#define ARKOI_LANGUAGE_IL_GENERATOR_H

#include <stack>

#include "visitor.h"
#include "tac.h"

class IRGenerator : public NodeVisitor {
public:
    IRGenerator() : _scopes(), _instructions(), _current_operand() {}

    void visit(const ProgramNode &node) override;

    void visit(const FunctionNode &node) override;

    void visit(const TypeNode &node) override;

    void visit(const BlockNode &node) override;

    void visit(const ParameterNode &node) override;

    void visit(const NumberNode &node) override;

    void visit(const ReturnNode &node) override;

    void visit(const IdentifierNode &node) override;

    [[nodiscard]] const std::vector<std::unique_ptr<Instruction>> &instructions() const { return _instructions; }

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    std::vector<std::unique_ptr<Instruction>> _instructions;
    Operand _current_operand;
};

#endif //ARKOI_LANGUAGE_IL_GENERATOR_H
