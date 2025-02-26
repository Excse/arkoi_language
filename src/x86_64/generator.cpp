#include "x86_64/generator.hpp"

#include "utils/utils.hpp"

#include "x86_64/register.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static const Register::Base TEMP_INT = Register::Base::R10;
static const Register::Base TEMP_SSE = Register::Base::XMM10;

std::stringstream Generator::generate(il::Module &module) {
    std::stringstream output;

    Generator generator;
    generator.visit(module);

    output << generator._text.rdbuf();
    output << generator._data.rdbuf();

    return output;
}

void Generator::visit(il::Module &module) {
    _text << ".intel_syntax noprefix\n";
    _text << ".section .text\n";
    _text << ".global _start\n";
    _text << "\n";

    _text << "_start:\n";
    _text << "\tcall main\n";
    _text << "\tmov rdi, rax\n";
    _text << "\tmov rax, 60\n";
    _text << "\tsyscall\n";
    _text << "\n";

    _data << ".section .data\n";

    for (auto &function: module) {
        function.accept(*this);
    }
}

void Generator::visit(il::Function &function) {
    _mapper = Mapper::map(function);
    _current_function = &function;

    _text << function.name() << ":\n";

    for (auto &block: function) {
        block.accept(*this);
    }
}

void Generator::visit(il::BasicBlock &block) {
    if (_current_function->entry() == &block) {
        _text << "\tenter " << _mapper.stack_size() << ", 0\n";
    } else {
        _text << block.label() << ":\n";
    }

    for (auto &instruction: block) {
        _text << "\t# ";
        instruction.accept(_printer);
        _text << "\n";

        instruction.accept(*this);
    }

    if (_current_function->exit() == &block) {
        _text << "\tleave\n";
        _text << "\tret\n";

        _text << "\n";
    }
}

void Generator::visit(il::Return &instruction) {
    // The destination is always either XMM0 or RAX, depending on the return type. Since the mapper already handles this
    // assignment, this instruction is simply translated into a mov instruction.

    auto destination = _map(instruction.result());
    auto source = _map(instruction.value());
    auto &type = instruction.result().type();
    _mov(source, destination, type);
}

void Generator::visit(il::Binary &) {}

void Generator::visit(il::Cast &) {}

void Generator::visit(il::Call &instruction) {
    // instruction.result() is unnecessary because the mapper always assigns the destination to either the XMM0 register
    // for floating-point values or the RDI register for integers.

    auto stack_arguments = Mapper::get_stack_parameters(instruction.arguments());
    auto stack_adjust = 8 * stack_arguments.size();
    stack_adjust = Mapper::align_size(stack_adjust);

    if (stack_adjust) {
        _text << "\tsub rsp, " << stack_adjust << "\n";
    }

    _text << "\tcall " << instruction.name() << "\n";

    if (stack_adjust) {
        _text << "\tadd rsp, " << stack_adjust << "\n";
    }
}

void Generator::visit(il::If &) {}

void Generator::visit(il::Goto &instruction) {
    _text << "\tjmp " << instruction.label() << "\n";
}

void Generator::visit(il::Store &instruction) {
    auto destination = _map(instruction.result());
    auto source = _map(instruction.value());
    auto &type = instruction.result().type();
    _mov(source, destination, type);
}

void Generator::visit(il::Load &instruction) {
    auto destination = _map(instruction.result());
    auto source = _map(instruction.value());
    auto &type = instruction.result().type();
    _mov(source, destination, type);
}

void Generator::visit(il::Constant &instruction) {
    // This instruction is generally unused if the optimizer runs. However, during a function call, it is retained since
    // the call instruction only accepts IL variables as arguments. Thus, most of the constant variables will be mapped
    // to a register or memory location according to the specific calling convention.

    auto destination = _map(instruction.result());
    auto source = _map(instruction.value());
    auto &type = instruction.result().type();
    _mov(source, destination, type);
}

void Generator::_mov(Operand source, const Operand &destination, const Type &type) {
    // If the source and destination is the same, the mov instruction is not needed.
    if (source == destination) return;

    // Since mov instructions only accept operands in the forms (src:dest) reg:mem, mem:reg, imm:reg, or imm:mem,
    // a mem:mem operation must be split into two mov instructions.
    if (std::holds_alternative<Memory>(source) && std::holds_alternative<Memory>(destination)) {
        if (std::holds_alternative<sem::Floating>(type)) {
            auto target = Register(TEMP_SSE, type.size());
            _mov(source, target, type);
            source = target;
        } else {
            auto target = Register(TEMP_INT, type.size());
            _mov(source, target, type);
            source = target;
        }
    }

    std::string menomic;
    if (std::holds_alternative<sem::Floating>(type)) {
        menomic = type.size() == Size::QWORD ? "movsd" : "movss";
    } else {
        menomic = "mov";
    }

    _text << "\t" << menomic << " " << destination << ", " << source << "\n";
}

Operand Generator::_map(const il::Operand &operand) {
    return std::visit(match{
        [&](const il::Immediate &immediate) -> Operand {
            if (std::holds_alternative<double>(immediate)) {
                auto name = "float" + std::to_string(_constants++);
                _data << "\t" << name << ": .double\t" << immediate << "\n";
                return Memory(Size::QWORD, name);
            } else if (std::holds_alternative<float>(immediate)) {
                auto name = "float" + std::to_string(_constants++);
                _data << "\t" << name << ": .float\t" << immediate << "\n";
                return Memory(Size::DWORD, name);
            } else {
                return immediate;
            }
        },
        [&](const il::Variable &variable) -> Operand {
            return _mapper[variable];
        },
    }, operand);
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
