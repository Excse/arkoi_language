//
// Created by timo on 8/18/24.
//

#include "il_generator.h"

#include <charconv>

#include "token.h"
#include "ast.h"

void IRGenerator::visit(const ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void IRGenerator::visit(const FunctionNode &node) {
    _instructions.emplace_back(TACLabel{std::string(node.name().value())});

    _scopes.push(node.table());
    node.block().accept(*this);
    _scopes.pop();
}

// Not used
void IRGenerator::visit(const TypeNode &node) {}

void IRGenerator::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

// Not used
void IRGenerator::visit(const ParameterNode &node) {}

void IRGenerator::visit(const NumberNode &node) {
    auto token_value = std::string(node.value().value());
    auto value = std::stoll(token_value);

    _current_operand = value;
}

void IRGenerator::visit(const ReturnNode &node) {
    // This will set _current_operand
    node.expression().accept(*this);

    _instructions.emplace_back(TACReturn{_current_operand});
}

void IRGenerator::visit(const IdentifierNode &node) {
    auto is_parameter = [](const Symbol &symbol) { return symbol.type() == Symbol::Type::Parameter; };

    auto scope = _scopes.top();
    auto symbol = scope->lookup(std::string(node.value().value()), is_parameter);

    _current_operand = symbol;
}
