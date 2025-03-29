#include "x86_64/mapper.hpp"

#include <array>

#include "utils/utils.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static std::array<Register::Base, 6> INT_REG_ORDER{
    Register::Base::DI, Register::Base::SI, Register::Base::D,
    Register::Base::C, Register::Base::R8, Register::Base::R9
};

static std::array<Register::Base, 8> SSE_REG_ORDER{
    Register::Base::XMM0, Register::Base::XMM1, Register::Base::XMM2, Register::Base::XMM3,
    Register::Base::XMM4, Register::Base::XMM5, Register::Base::XMM6, Register::Base::XMM7
};

static const Register RBP(Register::Base::BP, Size::QWORD);

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
            return _mappings.at(variable);
        },
        [&](const il::Memory &memory) -> Operand {
            return _mappings.at(memory);
        },
        [&](const il::Immediate &immediate) -> Operand {
            return std::visit([](const auto &value) -> Immediate { return value; }, immediate);
        }
    }, operand);
}

void Mapper::visit(il::Function &function) {
    _map_parameters(function.parameters());

    for (auto &block: function) {
        block.accept(*this);
    }

    int64_t local_offset = -8;
    for (auto &local: _locals) {
        auto size = local.type().size();
        _mappings.emplace(local, Memory(size, RBP, local_offset));
        local_offset -= (int64_t) size_to_bytes(size);
    }
}

void Mapper::_map_parameters(const std::vector<il::Variable> &parameters) {
    size_t stack = 0, integer = 0, floating = 0;

    for (auto &parameter: parameters) {
        const auto &type = parameter.type();

        if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
            if (integer < INT_REG_ORDER.size()) {
                auto reg = Register(INT_REG_ORDER[integer], type.size());
                _add_register(parameter, reg);
                integer++;
                continue;
            }
        } else if (std::holds_alternative<sem::Floating>(type)) {
            if (floating < SSE_REG_ORDER.size()) {
                auto reg = Register(SSE_REG_ORDER[floating], type.size());
                _add_register(parameter, reg);
                floating++;
                continue;
            }
        }

        auto offset = (int64_t) (16 + 8 * stack);
        auto memory = Memory(type.size(), RBP, offset);
        _add_memory(parameter, memory);
        stack++;
    }
}

void Mapper::visit(il::BasicBlock &block) {
    for (auto &instruction: block) {
        instruction.accept(*this);
    }
}

void Mapper::visit(il::Binary &instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Cast &instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Call &instruction) {
    auto result_reg = return_register(instruction.result().type());
    _add_register(instruction.result(), result_reg);
}

void Mapper::visit(il::Alloca &instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Load &instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Constant &instruction) {
    _add_local(instruction.result());
}

void Mapper::_add_local(const il::Operand &operand) {
    _locals.insert(operand);
}

void Mapper::_add_register(const il::Variable &variable, const Register &reg) {
    _locals.erase(variable);
    _mappings.emplace(variable, reg);
}

void Mapper::_add_memory(const il::Variable &variable, const Memory &memory) {
    _locals.erase(variable);
    _mappings.emplace(variable, memory);
}

size_t Mapper::stack_size() const {
    size_t stack_size = 0;

    for (const auto &local: _locals) {
        auto size = size_to_bytes(local.type().size());
        stack_size += size;
    }

    return align_size(stack_size);
}

Register Mapper::return_register(const sem::Type &type) {
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

size_t Mapper::align_size(size_t input) {
    static const size_t STACK_ALIGNMENT = 16;
    return (input + (STACK_ALIGNMENT - 1)) & ~(STACK_ALIGNMENT - 1);
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
