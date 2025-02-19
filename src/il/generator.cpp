#include "il/generator.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "ast/nodes.hpp"

using namespace arkoi::il;
using namespace arkoi;

Module Generator::generate(ast::Program &node) {
    Generator generator;
    node.accept(generator);
    return generator.module();
}

void Generator::visit(ast::Program &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void Generator::visit(ast::Function &node) {
    // Resetting the variables for each function
    _temp_index = 0;
    _allocas.clear();

    // Creates a new basic block that will get populated with instructions
    auto start_block = std::make_shared<BasicBlock>(node.name().value().contents() + "_entry");
    _current_block = start_block;

    // Creates the function end basic block
    auto end_block = std::make_shared<BasicBlock>(node.name().value().contents() + "_exit");

    auto &function = _module.emplace_back(node.name().symbol(), start_block, end_block);
    _current_function = &function;

    _result_temp = _make_temporary();
    _current_block->emplace_back<Alloca>(_result_temp, node.type());

    for (auto &parameter: node.parameters()) {
        auto alloca_temp = _make_temporary();
        _allocas.emplace(parameter.name().symbol(), alloca_temp);
        _current_block->emplace_back<Alloca>(alloca_temp, parameter.type());
    }

    for (auto &parameter: node.parameters()) {
        auto alloca_temp = _allocas.at(parameter.name().symbol());
        _current_block->emplace_back<Store>(alloca_temp, parameter.name().value().contents(), parameter.type());
    }

    node.block()->accept(*this);

    // Connect the last current block of this function with the end basic block
    _current_block->set_next(_current_function->exit());

    _current_block = _current_function->exit();

    auto result_temp = _make_temporary();
    _current_block->emplace_back<Load>(result_temp, _result_temp, node.type());
    _current_block->emplace_back<Return>(result_temp, node.type());
}

void Generator::visit(ast::Block &node) {
    for (const auto &statement: node.statements()) {
        statement->accept(*this);

        // Stop generating instructions for the block after a return statement.
        auto *_return = dynamic_cast<ast::Return *>(statement.get());
        if (_return) break;
    }
}

void Generator::visit(ast::Immediate &node) {
    switch (node.kind()) {
        case ast::Immediate::Kind::Integer: return visit_integer(node);
        case ast::Immediate::Kind::Floating: return visit_floating(node);
        case ast::Immediate::Kind::Boolean: return visit_boolean(node);
        default: throw std::runtime_error("This immediate type is not implemented yet.");
    }
}

void Generator::visit_integer(ast::Immediate &node) {
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

void Generator::visit_floating(ast::Immediate &node) {
    const auto &number_string = node.value().contents();

    auto value = std::stold(number_string);
    if (value > std::numeric_limits<float>::max()) {
        _current_operand = (double) value;
    } else {
        _current_operand = (float) value;
    }
}

void Generator::visit_boolean(ast::Immediate &node) {
    _current_operand = (node.value().type() == front::Token::Type::True);
}

void Generator::visit(ast::Return &node) {
    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    // Populate the current basic block with instructions
    _current_block->emplace_back<Store>(_result_temp, expression, node.type());
    _current_block->emplace_back<Goto>(_current_function->exit()->label());

    // Connect the current basic block with the function end basic block
    _current_block->set_next(_current_function->exit());
}

void Generator::visit(ast::Identifier &node) {
    if (node.kind() != ast::Identifier::Kind::Variable) {
        throw std::runtime_error("This kind of identifier is not yet implemented.");
    }

    // TODO: In the future there will be local/global and parameter variables,
    //       thus they need to be searched in such order: local, parameter, global.
    //       For now only parameter variables exist.
    const auto &variable = std::get<sem::Variable>(*node.symbol());

    auto alloca_temp = _allocas.at(node.symbol());
    auto temp = _make_temporary();
    _current_block->emplace_back<Load>(temp, alloca_temp, variable.type());
    _current_operand = temp;
}

void Generator::visit(ast::Binary &node) {
    // This will set _current_operand
    node.left()->accept(*this);
    auto left = _current_operand;

    // This will set _current_operand
    node.right()->accept(*this);
    auto right = _current_operand;

    auto type = Binary::node_to_instruction(node.op());
    auto result = _make_temporary();
    _current_operand = result;

    _current_block->emplace_back<Binary>(result, left, type, right, node.op_type(), node.result_type());
}

void Generator::visit(ast::Assign &node) {
    // TODO: In the future there will be local/global and parameter variables,
    //       thus they need to be searched in such order: local, parameter, global.
    //       For now only parameter variables exist.
    const auto &variable = std::get<sem::Variable>(*node.name().symbol());

    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto alloca_temp = _allocas.at(node.name().symbol());
    _current_block->emplace_back<Store>(alloca_temp, expression, variable.type());
}

void Generator::visit(ast::Cast &node) {
    // This will set _current_operand
    node.expression()->accept(*this);
    auto expression = _current_operand;

    auto result = _make_temporary();
    _current_operand = result;

    _current_block->emplace_back<Cast>(result, expression, node.from(), node.to());
}

void Generator::visit(ast::Call &node) {
    const auto &function = std::get<sem::Function>(*node.name().symbol());

    std::vector<Operand> arguments;
    for (const auto &argument: node.arguments()) {
        // This will set _current_operand
        argument->accept(*this);
        auto expression = _current_operand;

        arguments.push_back(std::move(expression));
    }

    auto result = _make_temporary();
    _current_operand = result;

    _current_block->emplace_back<Call>(result, function.name(), std::move(arguments), function.return_type());
}

void Generator::visit(ast::If &node) {
    auto branch_label = _make_label_symbol();
    auto branch_block = std::make_shared<BasicBlock>(branch_label);

    auto next_label = _make_label_symbol();
    auto next_block = std::make_shared<BasicBlock>(next_label);

    auto after_label = _make_label_symbol();
    auto after_block = std::make_shared<BasicBlock>(after_label);

    { // Entrance block
        // This will set _current_operand
        node.condition()->accept(*this);
        auto condition = _current_operand;

        _current_block->emplace_back<If>(condition, branch_label);

        _current_block->set_next(next_block);
        _current_block->set_branch(branch_block);
    }

    bool branch_already_connected = false;
    { // Branch block
        _current_block = branch_block;

        node.branch()->accept(*this);

        if (!_current_block->instructions().empty()) {
            auto &last_instruction = _current_block->instructions().back();
            if (std::holds_alternative<Goto>(last_instruction)) branch_already_connected = true;
        }

        if (!branch_already_connected) {
            _current_block->emplace_back<Goto>(after_label);
            _current_block->set_next(after_block);
        }
    }

    bool next_already_connected = false;
    { // Next block
        _current_block = next_block;

        if(node.next()) node.next()->accept(*this);

        if (!_current_block->instructions().empty()) {
            auto &last_instruction = _current_block->instructions().back();
            if (std::holds_alternative<Goto>(last_instruction)) next_already_connected = true;
        }

        if (!next_already_connected) {
            _current_block->emplace_back<Goto>(after_label);
            _current_block->set_next(after_block);
        }
    }

    if (!next_already_connected || !branch_already_connected) { // After block
        _current_block = after_block;
    }
}

std::string Generator::_make_label_symbol() {
    return "L" + to_string(_label_index++);
}

Variable Generator::_make_temporary() {
    return { "$", ++_temp_index };
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
