#include "x86_64/generator.hpp"

#include "utils/utils.hpp"

#include "x86_64/register.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

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
        _text << "\tpush rbp\n";
        _text << "\tmov rbp, rsp\n";
        if (_mapper.stack_size()) {
            _text << "\tsub rsp, " << _mapper.stack_size() << "\n";
        }
    } else {
        _text << block.label() << ":\n";
    }

    for (auto &instruction: block) {
        instruction.accept(*this);
    }

    if (_current_function->exit() == &block) {
        _text << "\tmov rsp, rbp\n";
        _text << "\tpop rbp\n";
        _text << "\tret\n";

        _text << "\n";
    }
}

void Generator::visit(il::Return &instruction) {
    auto &destination = _mapper[instruction.result()];
    auto source = _mapper[instruction.value()];
    auto &type = instruction.result().type();
    _mov(source, destination, type);
}

void Generator::visit(il::Binary &) {}

void Generator::visit(il::Cast &) {}

void Generator::visit(il::Call &instruction) {
    _text << "\tcall " << instruction.name() << "\n";
}

void Generator::visit(il::If &) {}

void Generator::visit(il::Goto &instruction) {
    _text << "\tjmp " << instruction.label() << "\n";
}

void Generator::visit(il::Store &) {}

void Generator::visit(il::Load &) {}

void Generator::visit(il::Constant &instruction) {
    auto &destination = _mapper[instruction.result()];
    auto &type = instruction.result().type();

    Operand source;
    if (auto *floating = std::get_if<sem::Floating>(&instruction.result().type())) {
        auto name = "float" + std::to_string(_constants++);
        auto size = floating->size() == Size::QWORD ? "double" : "float";
        _data << "\t" << name << ": ." << size << "\t" << instruction.value() << "\n";
        source = Memory(instruction.result().type().size(), name);
    } else {
        source = instruction.value();
    }

    _mov(source, destination, type);
}

void Generator::_mov(const Operand &source, const Operand &destination, const Type &type) {
    std::visit(match {
        [&](const sem::Integral &) {
            _text << "\tmov " << destination << ", " << source << "\n";
        },
        [&](const sem::Boolean &) {
            _text << "\tmov " << destination << ", " << source << "\n";
        },
        [&](const sem::Floating &type) {
            if(type.size() == Size::QWORD) {
                _text << "\tmovsd " << destination << ", " << source << "\n";
            } else if(type.size() == Size::DWORD) {
                _text << "\tmovss " << destination << ", " << source << "\n";
            }
        },
    }, type);
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
