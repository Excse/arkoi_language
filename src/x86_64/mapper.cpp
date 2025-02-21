#include "x86_64/mapper.hpp"

#include <array>

#include "utils/utils.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static constinit std::array<Register::Base, 6> INT_REG_ORDER{Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                             Register::Base::C, Register::Base::R8, Register::Base::R9};
static constinit std::array<Register::Base, 8> SSE_REG_ORDER{Register::Base::XMM0, Register::Base::XMM1,
                                                             Register::Base::XMM2, Register::Base::XMM3,
                                                             Register::Base::XMM4, Register::Base::XMM5,
                                                             Register::Base::XMM6, Register::Base::XMM7};

static const Register RBP(Register::Base::BP, Size::QWORD);

Mapper Mapper::map(il::Function &function) {
    Mapper mapper;
    mapper.visit(function);
    return mapper;
}

Mapping &Mapper::operator[](const il::Variable &variable) {
    return _mappings.at(variable);
}

Mapping Mapper::operator[](const il::Operand &operand) {
    return std::visit(match{
        [&](const il::Variable &variable) -> Mapping {
            return (*this)[variable];
        },
        [&](const il::Immediate &immediate) -> Mapping {
            return immediate;
        }
    }, operand);
}

void Mapper::visit(il::Function &function) {
    size_t int_index = 0, sse_index = 0;
    int64_t parameter_offset = 0;

    for (auto &parameter: function.parameters()) {
        auto result = _parameter_register(int_index, sse_index, parameter);
        if (result) {
            _add_register(parameter, *result);
        } else {
            auto size = parameter.type().size();
            _mappings[parameter] = Memory(size, RBP, parameter_offset);
            parameter_offset += (int64_t) size_to_bytes(size);
        }
    }

    for (auto &block: function) {
        block.accept(*this);
    }

    for (auto &variable: _stack_variables) {
        auto size = variable.type().size();
        _mappings[variable] = Memory(size, RBP, -_stack_size);
        _stack_size += (int64_t) size_to_bytes(size);
    }
}

void Mapper::visit(il::BasicBlock &block) {
    for (auto &instruction: block) {
        instruction.accept(*this);
    }
}

void Mapper::visit(il::Binary &instruction) {
    _add_stack(instruction.result());
}

void Mapper::visit(il::Cast &instruction) {
    _add_stack(instruction.result());
}

void Mapper::visit(il::Return &instruction) {
    auto reg = _return_register(instruction.result().type());
    _add_register(instruction.result(), reg);
}

void Mapper::visit(il::Call &instruction) {
    auto reg = _return_register(instruction.result().type());

    // Set the result register to the right
    _add_register(instruction.result(), reg);

    size_t int_index = 0, sse_index = 0;
    for (auto &argument: instruction.arguments()) {
        auto result = _parameter_register(int_index, sse_index, argument);
        if (result) {
            _add_register(argument, *result);
        } else {
            auto size = argument.type().size();
            _mappings[argument] = Memory(size, RBP, _stack_size);
            _stack_size -= (int64_t) size_to_bytes(size);
        }
    }
}

Register Mapper::_return_register(const Type &type) {
    return std::visit(match{
        [&](const sem::Integral &type) -> Register {
            return {Register::Base::A, type.size()};
        },
        [&](const sem::Floating &type) -> Register {
            return {Register::Base::XMM0, type.size()};
        },
        [&](const sem::Boolean &) -> Register {
            return {Register::Base::A, Size::BYTE};
        }
    }, type);
}

void Mapper::visit(il::Alloca &instruction) {
    _add_stack(instruction.result());
}

void Mapper::visit(il::Load &instruction) {
    _add_stack(instruction.result());
}

void Mapper::visit(il::Constant &instruction) {
    _add_stack(instruction.result());
}

std::optional<Register> Mapper::_parameter_register(size_t &int_index, size_t &sse_index, il::Variable &parameter) {
    return std::visit(match{
        [&](const sem::Integral &type) -> std::optional<Register> {
            if (int_index >= INT_REG_ORDER.size()) return std::nullopt;
            return Register(INT_REG_ORDER[int_index++], type.size());
        },
        [&](const sem::Floating &type) -> std::optional<Register> {
            if (sse_index >= SSE_REG_ORDER.size()) return std::nullopt;
            return Register(SSE_REG_ORDER[sse_index++], type.size());
        },
        [&](const sem::Boolean &type) -> std::optional<Register> {
            if (int_index >= INT_REG_ORDER.size()) return std::nullopt;
            return Register(INT_REG_ORDER[int_index++], type.size());
        }
    }, parameter.type());
}

void Mapper::_add_register(const il::Variable &variable, Register reg) {
    _stack_variables.erase(variable);
    _mappings[variable] = reg;
}

void Mapper::_add_stack(const il::Variable &variable) {
    _stack_variables.insert(variable);
}

std::ostream &operator<<(std::ostream &os, const Mapping &mapping) {
    std::visit([&](const auto &value) { os << value; }, mapping);
    return os;
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
