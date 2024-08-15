//
// Created by timo on 8/15/24.
//

#include "name_resolution.h"

#include "symbol_table.h"
#include "token.h"
#include "ast.h"

void NameResolution::visit(const Program &node) {
    _scopes.push(node.table());
    for (auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(const Function &node) {
    _check_non_existence(node.name());

    _scopes.push(node.table());
    for (const auto &item: node.parameters()) {
        item.accept(*this);
    }

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolution::visit(const Block &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(const Parameter &node) {
    _check_non_existence(node.name());
}

void NameResolution::visit(const Identifier &node) {
    _check_existence(node.value());
}

void NameResolution::visit(const Type &node) {}

void NameResolution::visit(const Number &node) {}

void NameResolution::visit(const Return &node) {
    node.expression().accept(*this);
}

void NameResolution::_check_non_existence(const Token &token) {
    try {
        auto scope = _scopes.top();
        scope->insert(std::string(token.value()));
    } catch (const IdentifierAlreadyTaken &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}

void NameResolution::_check_existence(const Token &token) {
    try {
        auto scope = _scopes.top();
        std::ignore = scope->lookup(std::string(token.value()));
    } catch (const IdentifierNotFound &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}
