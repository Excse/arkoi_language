#ifndef ARKOI_LANGUAGE_NAME_RESOLVER_H
#define ARKOI_LANGUAGE_NAME_RESOLVER_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolver : public NodeVisitor {
public:
    NameResolver() : _scopes(), _failed(false) {}

    void visit(const ProgramNode &node) override;

    void visit(const FunctionNode &node) override;

    void visit(const BlockNode &node) override;

    void visit(const ParameterNode &) override {};

    void visit(const IdentifierNode &node) override;

    void visit(const TypeNode &) override {};

    void visit(const NumberNode &) override {};

    void visit(const ReturnNode &node) override;

    void visit(const BinaryNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

    template<typename SymbolType, typename... Args>
    void _check_non_existence(const Token &token, Args &&... args);

    template<typename... SymbolTypes>
    void _check_existence(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    bool _failed;
};

#include "../src/name_resolver.tpp"

#endif //ARKOI_LANGUAGE_NAME_RESOLVER_H
