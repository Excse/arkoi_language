#include "x86_64/generator.hpp"

#include "x86_64/register.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

void Generator::visit(il::Module &) {
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
}

void Generator::visit(il::Function &function) {
    _mapper = Mapper::map(function);

    _text << function.name() << ":\n";

    _text << "\tpush rbp\n";
    _text << "\tmov rbp, rsp\n";
    if (_mapper.stack_size()) {
        _text << "\tsub rsp, " << _mapper.stack_size() << "\n";
    }

    for (auto &block: function) {
        block.accept(*this);
    }

    _text << "\tmov rsp, rbp\n";
    _text << "\tpop rbp\n";
    _text << "\tret\n";

    _text << "\n";
}

void Generator::visit(il::BasicBlock &block) {
    _text << block.label() << ":\n";

    for (auto &instruction: block) {
        instruction.accept(*this);
    }
}

void Generator::visit(il::Return &) {}

void Generator::visit(il::Binary &) {}

void Generator::visit(il::Cast &) {}

void Generator::visit(il::Call &) {}

void Generator::visit(il::If &) {}

void Generator::visit(il::Goto &) {}

void Generator::visit(il::Store &) {}

void Generator::visit(il::Load &) {}

void Generator::visit(il::Constant &) {}

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
