#include "back/x86_64/operand_resolver.hpp"

#include "mid/instruction.hpp"
#include "utils/utils.hpp"

using namespace arkoi::back::x86_64;

static constinit Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                             Register::Base::C, Register::Base::R8, Register::Base::R9};
static constinit Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                             Register::Base::XMM2, Register::Base::XMM3,
                                             Register::Base::XMM4, Register::Base::XMM5,
                                             Register::Base::XMM6, Register::Base::XMM7};
static constinit Register RBP = {Register::Base::BP, Size::QWORD};

static constexpr int64_t STACK_ALIGNMENT = 16;
static size_t CONSTANT_INDEX = 0;

OperandResolver OperandResolver::resolve(mid::Function &function) {
    OperandResolver resolver;

    function.linearize([&](auto &instruction) {
        instruction.accept(resolver);
    });

    return resolver;
}

void OperandResolver::visit(mid::Begin &instruction) {
    const auto &function = std::get<symbol::Function>(*instruction.function());

    size_t int_index = 0, sse_index = 0;
    for (auto &symbol: function.parameter_symbols()) {
        std::ignore = _resolve_parameter(symbol, int_index, sse_index);
    }
}

void OperandResolver::visit(mid::Return &instruction) {
    std::ignore = resolve_operand(instruction.value());
}

void OperandResolver::visit(mid::Binary &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.left());
    std::ignore = resolve_operand(instruction.right());
}

void OperandResolver::visit(mid::Cast &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.expression());
}

void OperandResolver::visit(mid::Call &instruction) {
    const auto &function = std::get<symbol::Function>(*instruction.function());

    size_t int_index = 0, sse_index = 0;
    for (auto &symbol: function.parameter_symbols()) {
        std::ignore = _resolve_parameter(symbol, int_index, sse_index);
    }

    std::ignore = resolve_operand(instruction.result());

    for (auto &argument: instruction.arguments()) {
        std::ignore = resolve_operand(argument);
    }
}

void OperandResolver::visit(mid::If &instruction) {
    std::ignore = resolve_operand(instruction.condition());
}

void OperandResolver::visit(mid::Store &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.value());
}

void OperandResolver::visit(mid::End &) {
    // Align the stack to comfort the specifications
    _local_size = (_local_size + STACK_ALIGNMENT - 1) & ~(STACK_ALIGNMENT - 1);
}

Operand OperandResolver::resolve_operand(const mid::Operand &operand) {
    return std::visit(match{
        [&](const mid::Variable &value) -> Operand { return _resolve_variable(value); },
        [&](const mid::Constant &value) -> Operand { return _resolve_constant(value); },
    }, operand);
}

Operand OperandResolver::_resolve_constant(const mid::Constant &constant) {
    if (!std::holds_alternative<float>(constant) && !std::holds_alternative<double>(constant)) return constant;

    auto result = _constants.find(constant);
    if (result != _constants.end()) return result->second.operand;

    auto data_name = ".LC" + std::to_string(CONSTANT_INDEX++);
    auto resolved = Memory(constant.size(), Memory::Address(data_name));

    _constants.emplace(constant, ConstantData{resolved, data_name});
    return resolved;
}

Operand OperandResolver::_resolve_variable(const mid::Variable &variable) {
    auto result = _resolved.find(variable);
    if (result != _resolved.end()) return result->second;

    Operand resolved = std::visit(match{
        [&](const symbol::Temporary &symbol) -> Operand { return _resolve_temporary(symbol); },
        // ParameterSymbols get resolved directly in the call and begin instruction.
        [](const auto &) -> Operand { std::unreachable(); }
    }, *variable.symbol());

    _resolved.emplace(variable, resolved);
    return resolved;
}

Operand OperandResolver::_resolve_temporary(const symbol::Temporary &symbol) {
    auto size = symbol.type().value().size();

    _local_size += (int64_t) size_to_bytes(size);
    auto resolved = Memory(symbol.type().value().size(), RBP, -_local_size);

    return resolved;
}

Operand OperandResolver::_resolve_parameter(const mid::Variable &variable,
                                                    size_t &int_index,
                                                    size_t &sse_index) {
    auto result = _resolved.find(variable);
    if (result != _resolved.end()) return result->second;

    const auto &parameter = std::get<symbol::Parameter>(*variable.symbol());

    auto resolved_register = _resolve_parameter_register(parameter, int_index, sse_index);
    if (resolved_register) {
        _resolved.emplace(variable, *resolved_register);
        return *resolved_register;
    }

    auto size = parameter.type().value().size();

    _parameter_offset += (int64_t) size_to_bytes(size);
    auto resolved = Memory(size, RBP, _parameter_offset);

    _resolved.emplace(variable, resolved);
    return resolved;
}

std::optional<Register> OperandResolver::_resolve_parameter_register(const symbol::Parameter &symbol,
                                                                     size_t &int_index,
                                                                     size_t &sse_index) {
    return std::visit(match{
        [&](const type::Integral &type) -> std::optional<Register> {
            if (int_index >= 6) return std::nullopt;

            return Register(INT_REG_ORDER[int_index++], type.size());
        },
        [&](const type::Boolean &) -> std::optional<Register> {
            if (int_index >= 6) return std::nullopt;

            return Register(INT_REG_ORDER[int_index], type::Boolean::size());
        },
        [&](const type::Floating &type) -> std::optional<Register> {
            if (sse_index >= 8) return std::nullopt;

            return Register(SSE_REG_ORDER[sse_index++], type.size());
        },
    }, symbol.type().value());
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
