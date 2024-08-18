//
// Created by timo on 8/18/24.
//

#ifndef ARKOI_LANGUAGE_IL_GENERATOR_H
#define ARKOI_LANGUAGE_IL_GENERATOR_H

#include <stack>

#include "visitor.h"
#include "tac.h"

class IRGenerator : public Visitor {
public:
    IRGenerator() : _scopes(), _instructions(), _current_operand() {}

    void visit(const Program &node) override;

    void visit(const Function &node) override;

    void visit(const Type &node) override;

    void visit(const Block &node) override;

    void visit(const Parameter &node) override;

    void visit(const Number &node) override;

    void visit(const Return &node) override;

    void visit(const Identifier &node) override;

    [[nodiscard]] const std::vector<Instruction> &instructions() const { return _instructions; }

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    std::vector<Instruction> _instructions;
    Operand _current_operand;
};


#endif //ARKOI_LANGUAGE_IL_GENERATOR_H
