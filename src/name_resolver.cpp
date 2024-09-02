#include "name_resolver.h"

#include "utils.h"
#include "ast.h"

void NameResolver::visit(const ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(const FunctionNode &node) {
    _check_non_existence<FunctionSymbol>(node.name());

    _scopes.push(node.table());

    for (size_t index = 0; index < node.parameters().size(); index++) {
        const auto &parameter = node.parameters()[index];
        _check_non_existence<ParameterSymbol>(parameter.name(), index);
    }

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolver::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(const IdentifierNode &node) {
    _check_existence<ParameterSymbol>(node.value());
}

void NameResolver::visit(const ReturnNode &node) {
    node.expression().accept(*this);
}

void NameResolver::visit(const BinaryNode &node) {
    node.left().accept(*this);
    node.right().accept(*this);
}
