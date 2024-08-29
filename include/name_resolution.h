#ifndef ARKOI_LANGUAGE_NAME_RESOLUTION_H
#define ARKOI_LANGUAGE_NAME_RESOLUTION_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolution : public NodeVisitor {
public:
    NameResolution() : _scopes(), _failed(false) {}

    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(TypeNode &node) override;

    void visit(NumberNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(BinaryNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

    template <typename SymbolType, typename... Args>
    void _check_non_existence(const Token &token, Args&&... args);

    void _check_existence(const Token &token, const std::function<bool(const Symbol &)> &predicate);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    bool _failed;
};

#include "../src/name_resolution.tpp"

#endif //ARKOI_LANGUAGE_NAME_RESOLUTION_H
