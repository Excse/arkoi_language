#include "semantic/name_resolver.hpp"

#include "frontend/ast.hpp"
#include "utils/utils.hpp"

using namespace arkoi::node;
using namespace arkoi;

NameResolver NameResolver::resolve(Program &node) {
    NameResolver resolver;

    node.accept(resolver);

    return resolver;
}

void NameResolver::visit(Program &node) {
    _scopes.push(node.table());

    // At first all function prototypes are name resolved.
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<Function *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }

    _scopes.pop();
}

void NameResolver::visit_as_prototype(Function &node) {
    auto symbol = _check_non_existence<FunctionSymbol>(node.name());
    node.set_symbol(symbol);
}

void NameResolver::visit(Function &node) {
    _scopes.push(node.table());

    std::vector<std::shared_ptr<Symbol>> parameters;
    for (auto &item: node.parameters()) {
        item.accept(*this);

        parameters.push_back(item.symbol());
    }

    auto &function = std::get<FunctionSymbol>(*node.symbol());
    function.set_parameters(std::move(parameters));

    node.block()->accept(*this);
    _scopes.pop();
}

void NameResolver::visit(Parameter &node) {
    auto parameter_symbol = _check_non_existence<ParameterSymbol>(node.name());
    node.set_symbol(parameter_symbol);
}

void NameResolver::visit(Block &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(Identifier &node) {
    auto symbol = _check_existence<ParameterSymbol>(node.value());
    node.set_symbol(symbol);
}

void NameResolver::visit(Return &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(Binary &node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void NameResolver::visit(Cast &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(If &node) {
    node.condition()->accept(*this);

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (node.branch()) std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
}

void NameResolver::visit(Call &node) {
    auto symbol = _check_existence<FunctionSymbol>(node.name());
    node.set_symbol(symbol);

    for (const auto &item: node.arguments()) {
        item->accept(*this);
    }
}