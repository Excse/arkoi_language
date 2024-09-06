#ifndef ARKOI_LANGUAGE_IL_GENERATOR_H
#define ARKOI_LANGUAGE_IL_GENERATOR_H

#include <stack>

#include "visitor.h"
#include "instruction.h"

class IRGenerator : public NodeVisitor {
public:
    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override {};

    void visit(NumberNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    [[nodiscard]] auto &instructions() const { return _instructions; }

private:
    Operand _make_temporary();

private:
    std::vector<std::unique_ptr<Instruction>> _instructions{};
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    Operand _current_operand{};
    size_t _temp_index{};
};

#endif //ARKOI_LANGUAGE_IL_GENERATOR_H
