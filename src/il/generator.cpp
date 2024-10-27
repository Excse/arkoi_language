#include "il/generator.hpp"

#include <limits>

#include "frontend/ast.hpp"
#include "utils/utils.hpp"

namespace il {

Generator Generator::generate(node::Program &node) {
    Generator generator;

    node.accept(generator);

    return generator;
}

void Generator::visit(node::Program &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void Generator::visit(node::Function &node) {
    // Create the function end symbol and basic block
    _function_end_block = std::make_shared<BasicBlock>();
    _function_end_symbol = _make_label_symbol();

    // Creates a new basic block that will get populated with instructions
    _current_block = std::make_shared<BasicBlock>();
    _functions.emplace_back(_current_block, _function_end_block);
    _current_block->emplace_back<Begin>(node.symbol());

    node.block()->accept(*this);

    // Connect the last current block of this function with the end basic block
    _current_block->set_next(_function_end_block);

    _current_block = _function_end_block;
    _function_end_block->emplace_back<Label>(_function_end_symbol);
    _function_end_block->emplace_back<End>();
}

void Generator::visit(node::Block &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void Generator::visit(node::Integer &node) {
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

void Generator::visit(node::Floating &node) {
    const auto &number_string = node.value().contents();

    auto value = std::stold(number_string);
    if (value > std::numeric_limits<float>::max()) {
        _current_operand = (double) value;
    } else {
        _current_operand = (float) value;
    }
}

void Generator::visit(node::Boolean &node) {
    _current_operand = (bool) node.value();
}

void Generator::visit(node::Return &node) {
    // This will set _current_operand
    node.expression()->accept(*this);

    // Populate the current basic block with instructions
    _current_block->emplace_back<Return>(std::move(_current_operand), node.type());
    _current_block->emplace_back<Goto>(_function_end_symbol);

    // Connect the current basic block with the function end basic block
    _current_block->set_next(_function_end_block);
}

void Generator::visit(node::Identifier &node) {
    _current_operand = node.symbol();
}

void Generator::visit(node::Binary &node) {
    // This will set _current_operand
    node.left()->accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right()->accept(*this);
    auto right = _current_operand;

    auto type = Binary::node_to_instruction(node.op());
    auto result = _make_temporary(node.type());
    _current_operand = result;

    _current_block->emplace_back<Binary>(result, left, type, right, node.type());
}

void Generator::visit(node::Cast &node) {
    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto result = _make_temporary(node.to());
    _current_operand = result;

    _current_block->emplace_back<Cast>(result, expression, node.from(), node.to());
}

void Generator::visit(node::Call &node) {
    const auto &function = std::get<symbol::Function>(*node.symbol());

    std::vector<Operand> arguments;
    for (const auto &argument: node.arguments()) {
        // This will set _current_operand
        argument->accept(*this);
        auto expression = _current_operand;

        arguments.push_back(std::move(expression));
    }

    auto result = _make_temporary(function.return_type().value());
    _current_operand = result;

    _current_block->emplace_back<Call>(result, node.symbol(), std::move(arguments));
}

void Generator::visit(node::If &node) {
    auto then_block = std::make_shared<BasicBlock>();
    auto then_label = _make_label_symbol();

    auto branch_block = std::make_shared<BasicBlock>();
    auto branch_label = _make_label_symbol();

    auto after_block = std::make_shared<BasicBlock>();
    auto after_label = _make_label_symbol();

    { // Entrance block
        // This will set _current_operand
        node.condition()->accept(*this);
        auto condition = _current_operand;

        _current_block->emplace_back<If>(condition, then_label);

        _current_block->set_next(then_block);
        _current_block->set_branch(branch_block);
    }

    { // Then block
        _current_block = then_block;
        then_block->emplace_back<Label>(then_label);
        std::visit([&](const auto &value) { value->accept(*this); }, node.then());

        _current_block->emplace_back<Goto>(after_label);
        _current_block->set_next(after_block);
    }

    { // Branch block
        _current_block = branch_block;
        branch_block->emplace_back<Label>(branch_label);
        if (node.branch()) {
            std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
        }

        _current_block->emplace_back<Goto>(after_label);
        _current_block->set_next(after_block);
    }

    { // After block
        _current_block = after_block;
        after_block->emplace_back<Label>(after_label);
    }
}

il::Variable Generator::_make_temporary(const Type &type) {
    auto temporary = std::make_shared<SymbolType>(symbol::Temporary("$", type));
    return { temporary, _temp_index++ };
}

Symbol Generator::_make_label_symbol() {
    auto name = "L" + to_string(_label_index++);
    return std::make_shared<SymbolType>(symbol::Temporary(name));
}

}
