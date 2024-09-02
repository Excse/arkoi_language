#include "il_generator.h"

#include "utils.h"
#include "ast.h"

void IRGenerator::visit(const ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void IRGenerator::visit(const FunctionNode &node) {
    auto symbol = _scopes.top()->lookup<FunctionSymbol>(to_string(node.name().value()));
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

void IRGenerator::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

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
    auto symbol = _scopes.top()->lookup<ParameterSymbol>(to_string(node.value().value()));
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

    auto name = "$tmp" + to_string(_temp_index);
    auto symbol = scope->insert<TemporarySymbol>(name);
    _temp_index++;

    return symbol;
}
