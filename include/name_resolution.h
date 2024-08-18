//
// Created by timo on 8/15/24.
//

#ifndef ARKOI_LANGUAGE_NAME_RESOLUTION_H
#define ARKOI_LANGUAGE_NAME_RESOLUTION_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolution : public Visitor {
public:
    NameResolution() : _scopes(), _failed(false) {}

    [[nodiscard]] bool has_failed() const { return _failed; }

private:
    void visit(const Program &node) override;

    void visit(const Function &node) override;

    void visit(const Block &node) override;

    void visit(const Parameter &node) override;

    void visit(const Identifier &node) override;

    void visit(const Type &node) override;

    void visit(const Number &node) override;

    void visit(const Return &node) override;

    void _check_non_existence(const Token &token, Symbol::Type type);

    void _check_existence(const Token &token, const std::function<bool(const Symbol &)> &predicate);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes;
    bool _failed;
};


#endif //ARKOI_LANGUAGE_NAME_RESOLUTION_H
