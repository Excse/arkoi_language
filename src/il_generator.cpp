#include "il_generator.hpp"

#include <limits>

#include "utils.hpp"
#include "ast.hpp"

IRGenerator IRGenerator::generate(ProgramNode &node) {
    IRGenerator generator;

    node.accept(generator);

    return generator;
}

void IRGenerator::visit(ProgramNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void IRGenerator::visit(FunctionNode &node) {
    _function_end = _make_label_symbol();
    _instructions.emplace_back(std::make_unique<BeginInstruction>(node.symbol()));

    node.block()->accept(*this);

    _instructions.emplace_back(std::make_unique<EndInstruction>(_function_end));
}

void IRGenerator::visit(BlockNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void IRGenerator::visit(IntegerNode &node) {
    const auto &number_string = node.value().contents();

    auto sign = !number_string.starts_with('-');
    if (sign) {
        auto value = std::stoll(number_string);
        if (value > std::numeric_limits<int32_t>::max()) {
            _current_operand = (int64_t) value;
        } else {
            _current_operand = (int32_t) value;
        }
    } else {
        auto value = std::stoull(number_string);
        if (value > std::numeric_limits<uint32_t>::max()) {
            _current_operand = (uint64_t) value;
        } else {
            _current_operand = (uint32_t) value;
        }
    }
}

void IRGenerator::visit(FloatingNode &node) {
    const auto &number_string = node.value().contents();

    auto value = std::stold(number_string);
    if (value > std::numeric_limits<float>::max()) {
        _current_operand = (double) value;
    } else {
        _current_operand = (float) value;
    }
}

void IRGenerator::visit(BooleanNode &node) {
    _current_operand = (bool) node.value();
}

void IRGenerator::visit(ReturnNode &node) {
    // This will set _current_operand
    node.expression()->accept(*this);

    _instructions.emplace_back(std::make_unique<ReturnInstruction>(std::move(_current_operand), node.type().value()));
    _instructions.emplace_back(std::make_unique<GotoInstruction>(_function_end));
}

void IRGenerator::visit(IdentifierNode &node) {
    _current_operand = node.symbol();
}

void IRGenerator::visit(BinaryNode &node) {
    // This will set _current_operand
    node.left()->accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right()->accept(*this);
    auto right = _current_operand;

    auto type = BinaryInstruction::node_to_instruction(node.op());
    auto result = _make_temporary(node.type().value());
    _current_operand = result;

    _instructions.emplace_back(std::make_unique<BinaryInstruction>(result, left, type, right, node.type().value()));
}

void IRGenerator::visit(CastNode &node) {
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto result = _make_temporary(node.to());
    _current_operand = result;

    _instructions.emplace_back(std::make_unique<CastInstruction>(result, expression, node.from().value(), node.to()));
}

void IRGenerator::visit(CallNode &node) {
    auto &function = std::get<FunctionSymbol>(*node.symbol());

    for (size_t index = 0; index < function.parameter_symbols().size(); index++) {
        auto &parameter = function.parameter_symbols()[index];

        auto &argument = node.arguments()[index];
        argument->accept(*this);
        auto expression = _current_operand;

        _instructions.emplace_back(std::make_unique<ArgumentInstruction>(std::move(expression), parameter));
    }

    auto result = _make_temporary(function.return_type().value());
    _current_operand = result;

    _instructions.emplace_back(std::make_unique<CallInstruction>(result, node.symbol()));
}

void IRGenerator::visit(IfNode &node) {
    auto after_label = _make_label_symbol();
    auto then_label = _make_label_symbol();
    auto else_label = _make_label_symbol();

    // This will set _current_operand
    node.condition()->accept(*this);
    auto condition = _current_operand;

    _instructions.push_back(std::make_unique<IfNotInstruction>(condition, else_label));

    _instructions.push_back(std::make_unique<LabelInstruction>(then_label));

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (auto &_else = node.els()) {
        _instructions.push_back(std::make_unique<GotoInstruction>(after_label));

        _instructions.push_back(std::make_unique<LabelInstruction>(else_label));

        std::visit([&](const auto &value) { value->accept(*this); }, *_else);

        _instructions.push_back(std::make_unique<LabelInstruction>(after_label));
    } else {
        _instructions.push_back(std::make_unique<LabelInstruction>(else_label));
    }
}

Operand IRGenerator::_make_temporary(const Type &type) {
    auto name = "$tmp" + to_string(_temp_index++);
    return std::make_shared<Symbol>(TemporarySymbol(name, type));
}

std::shared_ptr<Symbol> IRGenerator::_make_label_symbol() {
    auto name = "L" + to_string(_label_index++);
    return std::make_shared<Symbol>(TemporarySymbol(name));
}
