#include "il_generator.h"

#include "type_resolver.h"
#include "utils.h"
#include "ast.h"

void IRGenerator::visit(ProgramNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void IRGenerator::visit(FunctionNode &node) {
    _instructions.emplace_back(std::make_unique<LabelInstruction>(node.symbol()));

    _instructions.emplace_back(std::make_unique<BeginInstruction>());

    _scopes.push(node.table());
    node.block().accept(*this);
    _scopes.pop();

    _instructions.emplace_back(std::make_unique<EndInstruction>());
}

void IRGenerator::visit(BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void IRGenerator::visit(NumberNode &node) {
    auto number_string = to_string(node.value().value());
    auto sign = !number_string.starts_with('-');

    if (sign) {
        auto value = std::stoll(number_string);
        if (value > std::numeric_limits<int32_t>::max()) {
            _current_operand = Operand(Immediate((int64_t) value));
        } else {
            _current_operand = Operand(Immediate((int32_t) value));
        }
    } else {
        auto value = std::stoull(number_string);
        if (value > std::numeric_limits<uint32_t>::max()) {
            _current_operand = Operand(Immediate((uint64_t) value));
        } else {
            _current_operand = Operand(Immediate((uint32_t) value));
        }
    }
}

void IRGenerator::visit(ReturnNode &node) {
    // This will set _current_operand
    node.expression()->accept(*this);

    _instructions.emplace_back(std::make_unique<ReturnInstruction>(std::move(_current_operand)));
}

void IRGenerator::visit(IdentifierNode &node) {
    _current_operand = Operand(node.symbol());
}

void IRGenerator::visit(BinaryNode &node) {
    // This will set _current_operand
    node.left()->accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right()->accept(*this);
    auto right = _current_operand;

    auto type = BinaryInstruction::node_to_instruction(node.type());
    auto result = _make_temporary();
    _current_operand = result;

    _instructions.emplace_back(std::make_unique<BinaryInstruction>(result, left, type, right));
}

void IRGenerator::visit(CastNode &node) {
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto type = TypeResolver::_resolve_type(node.to());
    auto result = _make_temporary();
    _current_operand = result;

    _instructions.emplace_back(std::make_unique<CastInstruction>(result, type, expression));
}

Operand IRGenerator::_make_temporary() {
    auto scope = _scopes.top();

    auto name = "$tmp" + to_string(_temp_index);
    auto symbol = scope->insert<TemporarySymbol>(name);
    _temp_index++;

    return Operand(symbol);
}
