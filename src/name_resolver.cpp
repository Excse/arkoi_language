#include "name_resolver.h"

#include "utils.h"
#include "ast.h"

void NameResolver::visit(ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(FunctionNode &node) {
    auto function_symbol = _check_non_existence<FunctionSymbol>(node.name());
    node.set_symbol(function_symbol);

    _scopes.push(node.table());

    for (size_t index = 0; index < node.parameters().size(); index++) {
        auto &parameter = node.parameters()[index];
        auto parameter_symbol = _check_non_existence<ParameterSymbol>(parameter.name(), index);
        parameter.set_symbol(parameter_symbol);
    }

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolver::visit(BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(IdentifierNode &node) {
    auto symbol = _check_existence<ParameterSymbol>(node.value());
    node.set_symbol(symbol);
}

void NameResolver::visit(ReturnNode &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(BinaryNode &node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void NameResolver::visit(CastNode &node) {
    node.expression()->accept(*this);
}
