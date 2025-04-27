#include "x86_64/assembly.hpp"

#include "il/instruction.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

std::ostream &operator<<(std::ostream &os, const Label &label) {
    os << label.name() << ":";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Directive &directive) {
    os << directive.text();
    return os;
}

std::ostream &operator<<(std::ostream &os, const Instruction::Opcode &opcode) {
    switch (opcode) {
        case Instruction::Opcode::ADD: return os << "add";
        case Instruction::Opcode::CALL: return os << "call";
        case Instruction::Opcode::MOV: return os << "mov";
        case Instruction::Opcode::SYSCALL: return os << "syscall";
        case Instruction::Opcode::ENTER: return os << "enter";
        case Instruction::Opcode::LEAVE: return os << "leave";
        case Instruction::Opcode::RET: return os << "ret";
        case Instruction::Opcode::ADDSD: return os << "addsd";
        case Instruction::Opcode::ADDSS: return os << "addss";
        case Instruction::Opcode::SUBSD: return os << "subsd";
        case Instruction::Opcode::SUBSS: return os << "subss";
        case Instruction::Opcode::SUB: return os << "sub";
        case Instruction::Opcode::MULSD: return os << "mulsd";
        case Instruction::Opcode::MULSS: return os << "mulss";
        case Instruction::Opcode::IMUL: return os << "imul";
        case Instruction::Opcode::DIVSD: return os << "divsd";
        case Instruction::Opcode::DIVSS: return os << "divss";
        case Instruction::Opcode::IDIV: return os << "idiv";
        case Instruction::Opcode::DIV: return os << "div";
        case Instruction::Opcode::UCOMISD: return os << "ucomisd";
        case Instruction::Opcode::UCOMISS: return os << "ucomiss";
        case Instruction::Opcode::SETA: return os << "seta";
        case Instruction::Opcode::CMP: return os << "cmp";
        case Instruction::Opcode::SETG: return os << "setg";
        case Instruction::Opcode::SETB: return os << "setb";
        case Instruction::Opcode::SETL: return os << "setl";
        case Instruction::Opcode::CVTSS2SD: return os << "cvtss2sd";
        case Instruction::Opcode::CVTSD2SS: return os << "cvtsd2ss";
        case Instruction::Opcode::MOVSXD: return os << "movsxd";
        case Instruction::Opcode::MOVSX: return os << "movsx";
        case Instruction::Opcode::MOVZX: return os << "movzx";
        case Instruction::Opcode::CVTTSD2SI: return os << "cvttsd2si";
        case Instruction::Opcode::CVTTSS2SI: return os << "cvttss2si";
        case Instruction::Opcode::XORPS: return os << "xorps";
        case Instruction::Opcode::SETNE: return os << "setne";
        case Instruction::Opcode::SETP: return os << "setp";
        case Instruction::Opcode::OR: return os << "or";
        case Instruction::Opcode::CVTSI2SD: return os << "cvtsi2sd";
        case Instruction::Opcode::CVTSI2SS: return os << "cvtsi2ss";
        case Instruction::Opcode::TEST: return os << "test";
        case Instruction::Opcode::JNZ: return os << "jnz";
        case Instruction::Opcode::JMP: return os << "jmp";
        case Instruction::Opcode::MOVSD: return os << "movsd";
        case Instruction::Opcode::MOVSS: return os << "movss";
        default: return os;
    }
}

std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
    os << "\t" << instruction.opcode();
    for (size_t index = 0; index < instruction.operands().size(); index++) {
        const auto &operand = instruction.operands()[index];
        if (index != 0) {
            os << ", " << operand;
        } else {
            os << " " << operand;
        }
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const AssemblyItem &item) {
    std::visit([&os](const auto &value) { os << value; }, item);
    return os;
}

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
