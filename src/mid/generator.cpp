#include "mid/generator.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "def/ast.hpp"

using namespace arkoi::mid;
using namespace arkoi;

Generator Generator::generate(ast::Program &node) {
    Generator generator;

    node.accept(generator);

    return generator;
}

void Generator::visit(ast::Program &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void Generator::visit(ast::Function &node) {
    // Resetting the temp index so it is not going to be too huge
    _temp_index = 0;

    // Creates a new basic block that will get populated with instructions
    auto start_block = std::make_shared<BasicBlock>(_make_label_symbol());
    _current_block = start_block;

    // Creates the function end basic block
    auto end_block = std::make_shared<BasicBlock>(_make_label_symbol());

    auto &function = _module.functions().emplace_back(node.symbol(), start_block, end_block);
    _current_function = &function;

    node.block()->accept(*this);

    // Connect the last current block of this function with the end basic block
    _current_block->set_next(_current_function->end());

    _current_block = _current_function->end();
    _current_block->emplace<Label>(_current_function->end()->symbol());
}

void Generator::visit(ast::Block &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void Generator::visit(ast::Integer &node) {
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

void Generator::visit(ast::Floating &node) {
    const auto &number_string = node.value().contents();

    auto value = std::stold(number_string);
    if (value > std::numeric_limits<float>::max()) {
        _current_operand = (double) value;
    } else {
        _current_operand = (float) value;
    }
}

void Generator::visit(ast::Boolean &node) {
    _current_operand = (bool) node.value();
}

void Generator::visit(ast::Return &node) {
    // This will set _current_operand
    node.expression()->accept(*this);

    // Populate the current basic block with instructions
    _current_block->emplace<Return>(std::move(_current_operand), node.type());
    _current_block->emplace<Goto>(_current_function->end()->symbol());

    // Connect the current basic block with the function end basic block
    _current_block->set_next(_current_function->end());
}

void Generator::visit(ast::Identifier &node) {
    _current_operand = node.symbol();
}

void Generator::visit(ast::Binary &node) {
    // This will set _current_operand
    node.left()->accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right()->accept(*this);
    auto right = _current_operand;

    auto type = Binary::node_to_instruction(node.op());
    auto result = _make_temporary(node.result_type());
    _current_operand = result;

    _current_block->emplace<Binary>(result, left, type, right, node.op_type(), node.result_type());
}

void Generator::visit(ast::Assign &node) {
    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    _current_block->emplace<Store>(node.symbol(), expression, node.type());
}

void Generator::visit(ast::Cast &node) {
    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto result = _make_temporary(node.to());
    _current_operand = result;

    _current_block->emplace<Cast>(result, expression, node.from(), node.to());
}

void Generator::visit(ast::Call &node) {
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

    _current_block->emplace<Call>(result, node.symbol(), std::move(arguments));
}

void Generator::visit(ast::If &node) {
    auto then_label = _make_label_symbol();
    auto then_block = std::make_shared<BasicBlock>(then_label);

    auto branch_label = _make_label_symbol();
    auto branch_block = std::make_shared<BasicBlock>(branch_label);

    auto after_label = _make_label_symbol();
    auto after_block = std::make_shared<BasicBlock>(after_label);

    { // Entrance block
        // This will set _current_operand
        node.condition()->accept(*this);
        auto condition = _current_operand;

        _current_block->emplace<If>(condition, then_label);

        _current_block->set_next(then_block);
        _current_block->set_branch(branch_block);
    }

    { // ThenType block
        _current_block = then_block;
        then_block->emplace<Label>(then_label);
        std::visit([&](const auto &value) { value->accept(*this); }, node.then());

        _current_block->emplace<Goto>(after_label);
        _current_block->set_next(after_block);
    }

    { // Branch block
        _current_block = branch_block;
        branch_block->emplace<Label>(branch_label);
        if (node.branch()) {
            std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
        }

        _current_block->emplace<Goto>(after_label);
        _current_block->set_next(after_block);
    }

    { // After block
        _current_block = after_block;
        after_block->emplace<Label>(after_label);
    }
}

mid::Variable Generator::_make_temporary(const Type &type) {
    auto temporary = std::make_shared<SymbolType>(symbol::Temporary("$", type));
    return { temporary, ++_temp_index };
}

SharedSymbol Generator::_make_label_symbol() {
    auto name = "L" + to_string(_label_index++);
    return std::make_shared<SymbolType>(symbol::Temporary(name));
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
