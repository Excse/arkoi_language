#pragma once

#include <ostream>
#include <variant>
#include <vector>

#include "x86_64/operand.hpp"

namespace arkoi::x86_64 {

class Label {
public:
    explicit Label(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::string _name;
};

class Directive {
public:
    explicit Directive(std::string text) : _text(std::move(text)) {}

    [[nodiscard]] auto &text() const { return _text; }

private:
    std::string _text;
};

class Instruction {
public:
    enum class Opcode {
        CALL, MOV, SYSCALL, ENTER, LEAVE, RET, ADDSD, ADDSS, ADD, SUBSD, SUBSS, SUB, MULSD, MULSS, IMUL, DIVSD, DIVSS,
        IDIV, DIV, UCOMISD, UCOMISS, SETA, CMP, SETG, SETB, SETL, CVTSS2SD, CVTSD2SS, MOVSXD, MOVSX, MOVZX, CVTTSD2SI,
        CVTTSS2SI, XORPS, SETNE, SETP, OR, CVTSI2SD, CVTSI2SS, TEST, JNZ, JMP, MOVSD, MOVSS
    };

public:
    Instruction(Opcode opcode, std::vector<Operand> operands) : _operands(std::move(operands)), _opcode(opcode) {}

    [[nodiscard]] auto &operands() const { return _operands; }

    [[nodiscard]] auto &opcode() const { return _opcode; }

private:
    std::vector<Operand> _operands;
    Opcode _opcode;
};

struct AssemblyItem final : std::variant<Label, Directive, Instruction> {
    using variant::variant;
};

} // namespace arkoi::x86_64

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Label &label);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Directive &directive);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Instruction::Opcode &opcode);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Instruction &instruction);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::AssemblyItem &item);

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
