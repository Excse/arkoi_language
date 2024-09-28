#include "name_resolver.h"

#include "ast.h"

NameResolver NameResolver::resolve(ProgramNode &node) {
    NameResolver resolver;

    node.accept(resolver);

    return resolver;
}

void NameResolver::visit(ProgramNode &node) {
    _scopes.push(node.table());

    // At first all function prototypes are name resolved.
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<FunctionNode *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }

    _scopes.pop();
}

void NameResolver::visit_as_prototype(FunctionNode &node) {
    auto symbol = _check_non_existence<FunctionSymbol>(node.name());
    node.set_symbol(symbol);
}

void NameResolver::visit(FunctionNode &node) {
    _scopes.push(node.table());

    std::vector<std::shared_ptr<Symbol>> parameters;
    for (auto &item: node.parameters()) {
        item.accept(*this);

        parameters.push_back(item.symbol());
    }

    auto &function = std::get<FunctionSymbol>(*node.symbol());
    function.set_parameters(std::move(parameters));

    node.block().accept(*this);
    _scopes.pop();
}

void NameResolver::visit(ParameterNode &node) {
    auto parameter_symbol = _check_non_existence<ParameterSymbol>(node.name());
    node.set_symbol(parameter_symbol);
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

void NameResolver::visit(CallNode &node) {
    auto symbol = _check_existence<FunctionSymbol>(node.name());
    node.set_symbol(symbol);
}
