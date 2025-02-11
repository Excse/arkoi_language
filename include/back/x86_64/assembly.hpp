#pragma once

#include <sstream>

#include "back/x86_64/operand.hpp"
#include "def/symbol.hpp"

namespace arkoi::back::x86_64 {

class Assembly {
public:
    void cvttsd2si(const Operand &destination, const Operand &src);

    void cvttss2si(const Operand &destination, const Operand &src);

    void cvtss2sd(const Operand &destination, const Operand &src);

    void cvtsd2ss(const Operand &destination, const Operand &src);

    void cvtsi2ss(const Operand &destination, const Operand &src);

    void cvtsi2sd(const Operand &destination, const Operand &src);

    void movsx(const Operand &destination, const Operand &src);

    void movzx(const Operand &destination, const Operand &src);

    void movsxd(const Operand &destination, const Operand &src);

    void movss(const Operand &destination, const Operand &src);

    void movsd(const Operand &destination, const Operand &src);

    void mov(const Operand &destination, const Operand &src);

    void label(const std::string &name, bool newline = true);

    void label(const SharedSymbol &symbol);

    void jmp(const SharedSymbol &destination);

    void jne(const SharedSymbol &destination);

    void pop(const Operand &destination);

    void push(const Operand &src);

    void ret();

    void ucomiss(const Operand &destination, const Operand &source);

    void ucomisd(const Operand &destination, const Operand &source);

    void pxor(const Operand &destination, const Operand &source);

    void cmp(const Operand &first, const Operand &second);

    void setne(const Operand &destination);

    void add(const Operand &destination, const Operand &src);

    void addsd(const Operand &destination, const Operand &src);

    void addss(const Operand &destination, const Operand &src);

    void sub(const Operand &destination, const Operand &src);

    void subsd(const Operand &destination, const Operand &src);

    void subss(const Operand &destination, const Operand &src);

    void idiv(const Operand &dividend);

    void div(const Operand &dividend);

    void divsd(const Operand &destination, const Operand &src);

    void divss(const Operand &destination, const Operand &src);

    void imul(const Operand &destination, const Operand &src);

    void mulsd(const Operand &destination, const Operand &src);

    void mulss(const Operand &destination, const Operand &src);

    void directive(const std::string &name, const std::vector<std::string> &settings = {});

    void call(const std::string &name);

    void call(const SharedSymbol &symbol);

    void syscall();

    void comment(const std::string &comment);

    void newline();

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};

} // namespace arkoi::back::x86_64

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
