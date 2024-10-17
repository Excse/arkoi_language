#include "intermediate/generator.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "frontend/ast.hpp"

using namespace arkoi::intermediate;
using namespace arkoi;

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
    _cfgs.emplace_back(_current_block, _function_end_block);
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
    _current_block->emplace_back<Return>(std::move(_current_operand), node.type().value());
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
    auto result = _make_temporary(node.type().value());
    _current_operand = result;

    _current_block->emplace_back<Binary>(result, left, type, right, node.type().value());
}

void Generator::visit(node::Cast &node) {
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto result = _make_temporary(node.to());
    _current_operand = result;

    _current_block->emplace_back<Cast>(result, expression, node.from().value(), node.to());
}

void Generator::visit(node::Call &node) {
    const auto &function = std::get<FunctionSymbol>(*node.symbol());

    for (size_t index = 0; index < function.parameter_symbols().size(); index++) {
        const auto &parameter = function.parameter_symbols()[index];

        const auto &argument = node.arguments()[index];
        argument->accept(*this);
        auto expression = _current_operand;

        _current_block->emplace_back<Argument>(std::move(expression), parameter);
    }

    auto result = _make_temporary(function.return_type().value());
    _current_operand = result;

    _current_block->emplace_back<Call>(result, node.symbol());
}

void Generator::visit(node::If &node) {
    // At first the condition will be evaluated and then an "if not" instruction is inserted into the current basic block
    node.condition()->accept(*this);
    auto condition = _current_operand;

    // The basic blocks and labels that are necessary for an if statement
    auto after_label = _make_label_symbol();
    auto after_block = std::make_shared<BasicBlock>();

    auto then_label = _make_label_symbol();
    auto then_block = std::make_shared<BasicBlock>();
    then_block->set_next(after_block);
    _current_block->set_next(then_block);

    std::shared_ptr<BasicBlock> branch_block;
    std::shared_ptr<Symbol> branch_label;
    if (node.branch()) {
        branch_label = _make_label_symbol();
        branch_block = std::make_shared<BasicBlock>();

        _current_block->set_branch(branch_block);
        branch_block->set_next(after_block);
    } else {
        branch_label = after_label;
        branch_block = after_block;

        _current_block->set_branch(after_block);
    }

    _current_block->emplace_back<IfNot>(condition, branch_label);

    // Generate the instructions for the then basic block
    _current_block = then_block;
    then_block->emplace_back<Label>(then_label);

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (node.branch()) {
        then_block->emplace_back<Goto>(after_label);

        // Generate the instructions for the branch basic block
        _current_block = branch_block;
        branch_block->emplace_back<Label>(branch_label);

        std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
    }

    _current_block = after_block;
    after_block->emplace_back<Label>(after_label);
}

Operand Generator::_make_temporary(const Type &type) {
    auto name = "$tmp" + to_string(_temp_index++);
    return std::make_shared<Symbol>(TemporarySymbol(name, type));
}

std::shared_ptr<Symbol> Generator::_make_label_symbol() {
    auto name = "L" + to_string(_label_index++);
    return std::make_shared<Symbol>(TemporarySymbol(name));
}
