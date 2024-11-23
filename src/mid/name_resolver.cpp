#include "mid/name_resolver.hpp"

#include "utils/utils.hpp"
#include "def/ast.hpp"

using namespace arkoi::mid;

NameResolver NameResolver::resolve(ast::Program &node) {
    NameResolver resolver;

    node.accept(resolver);

    return resolver;
}

void NameResolver::visit(ast::Program &node) {
    _scopes.push(node.table());

    // At first all function prototypes are name resolved.
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<ast::Function *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }

    _scopes.pop();
}

void NameResolver::visit_as_prototype(ast::Function &node) {
    auto symbol = _check_non_existence<symbol::Function>(node.name());
    node.set_symbol(symbol);
}

void NameResolver::visit(ast::Function &node) {
    _scopes.push(node.table());

    std::vector<SharedSymbol> parameters;
    for (auto &item: node.parameters()) {
        item.accept(*this);

        parameters.push_back(item.symbol());
    }

    auto &function = std::get<symbol::Function>(*node.symbol());
    function.set_parameters(std::move(parameters));

    node.block()->accept(*this);
    _scopes.pop();
}

void NameResolver::visit(ast::Parameter &node) {
    auto parameter_symbol = _check_non_existence<symbol::Parameter>(node.name());
    node.set_symbol(parameter_symbol);
}

void NameResolver::visit(ast::Block &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(ast::Identifier &node) {
    auto symbol = _check_existence<symbol::Parameter>(node.value());
    node.set_symbol(symbol);
}

void NameResolver::visit(ast::Return &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(ast::Binary &node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void NameResolver::visit(ast::Cast &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(ast::If &node) {
    node.condition()->accept(*this);

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (node.branch()) std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
}

void NameResolver::visit(ast::Call &node) {
    auto symbol = _check_existence<symbol::Function>(node.name());
    node.set_symbol(symbol);

    for (const auto &item: node.arguments()) {
        item->accept(*this);
    }
}
