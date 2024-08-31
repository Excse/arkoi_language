#ifndef ARKOI_LANGUAGE_NAME_RESOLUTION_H
#define ARKOI_LANGUAGE_NAME_RESOLUTION_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolution : public NodeVisitor {
public:
    NameResolution() : _scopes(), _failed(false) {}

    void visit(const ProgramNode &node) override;

    void visit(const FunctionNode &node) override;

    void visit(const BlockNode &node) override;

    void visit(const ParameterNode &node) override;

    void visit(const IdentifierNode &node) override;

    void visit(const TypeNode &node) override;

    void visit(const NumberNode &node) override;

    void visit(const ReturnNode &node) override;

    void visit(const BinaryNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

    template<typename SymbolType, typename... Args>
    void _check_non_existence(const Token &token, Args &&... args);

    void _check_existence(const Token &token, const std::function<bool(const Symbol &)> &predicate);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    bool _failed;
};

#include "../src/name_resolution.tpp"

#endif //ARKOI_LANGUAGE_NAME_RESOLUTION_H
