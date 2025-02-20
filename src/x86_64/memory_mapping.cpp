#include "x86_64/memory_mapping.hpp"

#include "utils/utils.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static constinit Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                    Register::Base::C, Register::Base::R8, Register::Base::R9};
static constinit Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                                    Register::Base::XMM2, Register::Base::XMM3,
                                                    Register::Base::XMM4, Register::Base::XMM5,
                                                    Register::Base::XMM6, Register::Base::XMM7};

MemoryMapper MemoryMapper::map(il::Function &function) {
    MemoryMapper mapper;
    mapper.visit(function);
    return mapper;
}

void MemoryMapper::visit(il::Function &function) {
    for (auto &block: function) {
        block.accept(*this);
    }
}

void MemoryMapper::visit(il::BasicBlock &block) {
    for (auto &instruction: block) {
        instruction.accept(*this);
    }
}

void MemoryMapper::visit(il::Binary &instruction) {
    _add_stack(instruction.result());
}

void MemoryMapper::visit(il::Cast &instruction) {
    _add_stack(instruction.result());
}

void MemoryMapper::visit(il::Call &instruction) {
    auto reg = std::visit(match{
        [&](const sem::Integral &type) -> Register {
            return {Register::Base::A, type.size()};
        },
        [&](const sem::Floating &type) -> Register {
            return {Register::Base::XMM0, type.size()};
        },
        [&](const sem::Boolean &) -> Register {
            return {Register::Base::A, Size::BYTE};
        }
    }, instruction.result().type());

    // Set the result register to the right
    _add_register(instruction.result(), reg);

    size_t int_index = 0, sse_index = 0;
    for (auto &argument: instruction.arguments()) {
        _add_argument(int_index, sse_index, argument);
    }
}

void MemoryMapper::_add_argument(size_t &int_index, size_t &sse_index, il::Variable &argument) {
    std::visit(match{
        [&](const sem::Integral &type) {
            if (int_index >= 6) {
                _add_stack(argument);
            } else {
                auto reg = Register(INT_REG_ORDER[int_index], type.size());
                _add_register(argument, reg);
                int_index++;
            }
        },
        [&](const sem::Floating &type) {
            if (sse_index >= 8) {
                _add_stack(argument);
            } else {
                auto reg = Register(SSE_REG_ORDER[sse_index], type.size());
                _add_register(argument, reg);
                sse_index++;
            }
        },
        [&](const sem::Boolean &type) {
            if (int_index >= 6) {
                _add_stack(argument);
            } else {
                auto reg = Register(INT_REG_ORDER[int_index], type.size());
                _add_register(argument, reg);
                int_index++;
            }
        }
    }, argument.type());
}

void MemoryMapper::visit(il::Alloca &instruction) {
    _add_stack(instruction.result());
}

void MemoryMapper::visit(il::Load &instruction) {
    _add_stack(instruction.result());
}

void MemoryMapper::visit(il::Constant &instruction) {
    _add_stack(instruction.result());
}

Mapping &MemoryMapper::operator[](const il::Variable &variable) {
    if(!_mappings.contains(variable)) {
        throw std::invalid_argument("This variable is not mapped.");
    }
    return _mappings.at(variable);
}

void MemoryMapper::_add_register(const il::Variable &variable, Register reg) {
    _mappings[variable] = reg;
}

void MemoryMapper::_add_stack(const il::Variable &variable) {
    _mappings[variable] = _stack_size;
    _stack_size += size_to_bytes(variable.type().size());
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
