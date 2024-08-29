#include "name_resolution.h"

#include "utils.h"
#include "ast.h"

void NameResolution::visit(ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(FunctionNode &node) {
    _check_non_existence<FunctionSymbol>(node.name());

    _scopes.push(node.table());
    for (auto &item: node.parameters()) {
        item.accept(*this);
    }

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolution::visit(BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(ParameterNode &node) {
    _check_non_existence<ParameterSymbol>(node.name());
}

void NameResolution::visit(IdentifierNode &node) {
    auto is_parameter = [](const Symbol &symbol) { return std::holds_alternative<ParameterSymbol>(symbol); };
    _check_existence(node.value(), is_parameter);
}

void NameResolution::visit(TypeNode &) {}

void NameResolution::visit(NumberNode &) {}

void NameResolution::visit(ReturnNode &node) {
    node.expression().accept(*this);
}

void NameResolution::visit(BinaryNode &node) {
    node.left().accept(*this);
    node.right().accept(*this);
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
