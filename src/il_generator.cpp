#include "il_generator.h"

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
    auto is_function = [](const Symbol &symbol) { return symbol.type() == Symbol::Type::Function; };

    auto current_scope = _scopes.top();
    auto symbol = current_scope->lookup(std::string(node.name().value()), is_function);

    auto label = std::make_unique<LabelInstruction>(symbol);
    _instructions.emplace_back(std::move(label));

    auto begin = std::make_unique<BeginInstruction>();
    _instructions.emplace_back(std::move(begin));

    _scopes.push(node.table());
    node.block().accept(*this);
    _scopes.pop();

    auto end = std::make_unique<EndInstruction>();
    _instructions.emplace_back(std::move(end));
}

void IRGenerator::visit(const TypeNode &) {}

void IRGenerator::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void IRGenerator::visit(const ParameterNode &) {}

void IRGenerator::visit(const NumberNode &node) {
    auto value = std::string(node.value().value());
    _current_operand = std::stoll(value);
}

void IRGenerator::visit(const ReturnNode &node) {
    // This will set _current_operand
    node.expression().accept(*this);

    auto instruction = std::make_unique<ReturnInstruction>(std::move(_current_operand));
    _instructions.emplace_back(std::move(instruction));
}

void IRGenerator::visit(const IdentifierNode &node) {
    auto is_parameter = [](const Symbol &symbol) { return symbol.type() == Symbol::Type::Parameter; };

    auto current_scope = _scopes.top();
    auto symbol = current_scope->lookup(std::string(node.value().value()), is_parameter);

    _current_operand = symbol;
}

void IRGenerator::visit(const BinaryNode &node) {
    // This will set _current_operand
    node.left().accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right().accept(*this);
    auto right = _current_operand;

    auto type = BinaryInstruction::node_to_instruction(node.type());
    auto result = _make_temporary();

    auto instruction = std::make_unique<BinaryInstruction>(result, std::move(left), type, std::move(right));
    _instructions.emplace_back(std::move(instruction));

    _current_operand = result;
}

std::shared_ptr<Symbol> IRGenerator::_make_temporary() {
    auto scope = _scopes.top();

    auto name = "$tmp" + std::to_string(_temp_index);
    auto symbol = scope->insert(name, Symbol::Type::Temporary);
    _temp_index++;

    return symbol;
}
