#include "x86_64/mapper.hpp"

#include <cassert>
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
static const Register RSP(Register::Base::SP, Size::QWORD);

Mapper Mapper::map(il::Function &function) {
    Mapper mapper;
    mapper.visit(function);
    return mapper;
}

Operand &Mapper::operator[](const il::Variable &variable) {
    return _mappings.at(variable);
}

Operand Mapper::operator[](const il::Operand &operand) {
    return std::visit(match{
        [&](const il::Variable &variable) -> Operand {
            return (*this)[variable];
        },
        [&](const il::Immediate &immediate) -> Operand {
            return immediate;
        }
    }, operand);
}

void Mapper::visit(il::Function &function) {
    auto int_parameters = int_register_parameters(function.parameters());
    for (size_t index = 0; index < int_parameters.size(); index++) {
        auto &parameter = int_parameters.at(index);
        auto size = parameter.type().size();
        auto reg = Register(INT_REG_ORDER[index], size);
        _add_register(parameter, reg);
    }

    auto sse_parameters = sse_register_parameters(function.parameters());
    for (size_t index = 0; index < sse_parameters.size(); index++) {
        auto &parameter = sse_parameters.at(index);
        auto size = parameter.type().size();
        auto reg = Register(SSE_REG_ORDER[index], size);
        _add_register(parameter, reg);
    }

    auto stack_parameters = stack_parameters(function.parameters());
    size_t parameter_offset = 16 + (stack_parameters.size() * 8);
    for (auto &parameter: stack_parameters) {
        auto size = parameter.type().size();
        _mappings[parameter] = Memory(size, RBP, (int64_t) parameter_offset);
        parameter_offset -= 8;
    }

    for (auto &block: function) {
        block.accept(*this);
    }

    int64_t local_offset = -8;
    for (auto &variable: _locals) {
        auto size = variable.type().size();
        _mappings[variable] = Memory(size, RBP, local_offset);
        local_offset -= (int64_t) size_to_bytes(size);
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
    auto result_reg = _return_register(instruction.result().type());

    // Set the result register to the right
    _add_register(instruction.result(), result_reg);

    auto int_parameters = int_register_parameters(instruction.arguments());
    for (size_t index = 0; index < int_parameters.size(); index++) {
        auto &parameter = int_parameters.at(index);
        auto size = parameter.type().size();
        auto reg = Register(INT_REG_ORDER[index], size);
        _add_register(parameter, reg);
    }

    auto sse_parameters = sse_register_parameters(instruction.arguments());
    for (size_t index = 0; index < sse_parameters.size(); index++) {
        auto &parameter = sse_parameters.at(index);
        auto size = parameter.type().size();
        auto reg = Register(SSE_REG_ORDER[index], size);
        _add_register(parameter, reg);
    }

    auto stack_parameters = stack_parameters(instruction.arguments());
    size_t parameter_offset = 0;
    for (auto &parameter: stack_parameters) {
        auto size = parameter.type().size();
        _mappings[parameter] = Memory(size, RSP, (int64_t) parameter_offset);
        parameter_offset -= 8;
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

void Mapper::_add_register(const il::Variable &variable, Register reg) {
    _locals.erase(variable);
    _mappings[variable] = reg;
}

void Mapper::_add_stack(const il::Variable &variable) {
    _locals.insert(variable);
}

size_t Mapper::stack_size() const {
    size_t stack_size = 0;

    for (const auto &local: _locals) {
        auto size = size_to_bytes(local.type().size());
        stack_size += size;
    }

    static const size_t STACK_ALIGNMENT = 16;
    stack_size = (stack_size + (STACK_ALIGNMENT - 1)) & ~(STACK_ALIGNMENT - 1);

    return stack_size;
}

std::vector<il::Variable> Mapper::int_register_parameters(const std::vector<il::Variable> &parameters) {
    std::vector<il::Variable> result;

    for (auto &parameter: parameters) {
        if (std::holds_alternative<sem::Floating>(parameter.type())) continue;
        if (result.size() >= INT_REG_ORDER.size()) break;
        result.push_back(parameter);
    }

    return result;
}

std::vector<il::Variable> Mapper::sse_register_parameters(const std::vector<il::Variable> &parameters) {
    std::vector<il::Variable> result;

    for (auto &parameter: parameters) {
        if (!std::holds_alternative<sem::Floating>(parameter.type())) continue;
        if (result.size() >= SSE_REG_ORDER.size()) break;
        result.push_back(parameter);
    }

    return result;
}

std::vector<il::Variable> Mapper::stack_parameters(const std::vector<il::Variable> &parameters) {
    std::vector<il::Variable> result;

    size_t int_parameters = 0, sse_parameters = 0;
    for (auto &parameter: parameters) {
        if (std::holds_alternative<sem::Floating>(parameter.type())) {
            if (sse_parameters >= SSE_REG_ORDER.size()) {
                result.push_back(parameter);
            } else {
                sse_parameters++;
            }
        } else {
            if (int_parameters >= INT_REG_ORDER.size()) {
                result.push_back(parameter);
            } else {
                int_parameters++;
            }
        }
    }

    // Reverse the parameters that will be passed on the stack
    std::reverse(result.begin(), result.end());

    return result;
}

std::ostream &operator<<(std::ostream &os, const Operand &mapping) {
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
