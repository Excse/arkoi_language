#include "name_resolution.h"

#include "utils.h"
#include "ast.h"

void NameResolution::visit(const ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(const FunctionNode &node) {
    _check_non_existence<FunctionSymbol>(node.name());

    _scopes.push(node.table());

    for (size_t index = 0; index < node.parameters().size(); index++) {
        const auto &parameter = node.parameters()[index];
        _check_non_existence<ParameterSymbol>(parameter.name(), index);
    }

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolution::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolution::visit(const ParameterNode &) {
    throw std::invalid_argument("This is handled in visit(FunctionNode &), as we need the specific parameter index.");
}

void NameResolution::visit(const IdentifierNode &node) {
    auto is_parameter = [](const Symbol &symbol) { return std::holds_alternative<ParameterSymbol>(symbol); };
    _check_existence(node.value(), is_parameter);
}

void NameResolution::visit(const TypeNode &) {}

void NameResolution::visit(const NumberNode &) {}

void NameResolution::visit(const ReturnNode &node) {
    node.expression().accept(*this);
}

void NameResolution::visit(const BinaryNode &node) {
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
