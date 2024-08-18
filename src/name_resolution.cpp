#include "name_resolution.h"

#include "ast.h"

void NameResolution::visit(const Program &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(const Function &node) {
    _check_non_existence(node.name(), Symbol::Type::Function);

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
    _check_non_existence(node.name(), Symbol::Type::Parameter);
}

void NameResolution::visit(const Identifier &node) {
    auto is_parameter = [](const Symbol &symbol) { return symbol.type() == Symbol::Type::Parameter; };
    _check_existence(node.value(), is_parameter);
}

void NameResolution::visit(const Type &node) {}

void NameResolution::visit(const Number &node) {}

void NameResolution::visit(const Return &node) {
    node.expression().accept(*this);
}

void NameResolution::_check_non_existence(const Token &token, Symbol::Type type) {
    try {
        auto scope = _scopes.top();
        scope->insert(std::string(token.value()), type);
    } catch (const IdentifierAlreadyTaken &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}

void NameResolution::_check_existence(const Token &token, const std::function<bool(const Symbol &)> &predicate) {
    try {
        auto scope = _scopes.top();
        std::ignore = scope->lookup(std::string(token.value()), predicate);
    } catch (const IdentifierNotFound &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}
