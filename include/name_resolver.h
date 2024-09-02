#ifndef ARKOI_LANGUAGE_NAME_RESOLVER_H
#define ARKOI_LANGUAGE_NAME_RESOLVER_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolver : public NodeVisitor {
public:
    NameResolver() : _scopes(), _failed(false) {}

    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override {};

    void visit(IdentifierNode &node) override;

    void visit(TypeNode &) override {};

    void visit(NumberNode &) override {};

    void visit(ReturnNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

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
