//
// Created by timo on 8/18/24.
//

#include "il_generator.h"

#include <charconv>

#include "token.h"
#include "ast.h"

void IRGenerator::visit(const Program &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void IRGenerator::visit(const Function &node) {
    _instructions.emplace_back(TACLabel{std::string(node.name().value())});

    _scopes.push(node.table());
    node.block().accept(*this);
    _scopes.pop();
}

// Not used
void IRGenerator::visit(const Type &node) {}

void IRGenerator::visit(const Block &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

// Not used
void IRGenerator::visit(const Parameter &node) {}

void IRGenerator::visit(const Number &node) {
    auto token_value = std::string(node.value().value());
    auto value = std::stoll(token_value);

    _current_operand = value;
}

void IRGenerator::visit(const Return &node) {
    // This will set _current_operand
    node.expression().accept(*this);

    _instructions.emplace_back(TACReturn{_current_operand});
}

void IRGenerator::visit(const Identifier &node) {
    auto is_parameter = [](const Symbol &symbol) { return symbol.type() == Symbol::Type::Parameter; };

    auto scope = _scopes.top();
    auto symbol = scope->lookup(std::string(node.value().value()), is_parameter);

    _current_operand = symbol;
}
