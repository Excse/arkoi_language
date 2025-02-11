#include "back/x86_64/assembly.hpp"

#include <iomanip>

using namespace arkoi::back::x86_64;

void Assembly::cvttsd2si(const Operand &destination, const Operand &src) {
    _output << "\tcvttsd2si " << destination << ", " << src << "\n";
}

void Assembly::cvttss2si(const Operand &destination, const Operand &src) {
    _output << "\tcvttss2si " << destination << ", " << src << "\n";
}

void Assembly::cvtss2sd(const Operand &destination, const Operand &src) {
    _output << "\tcvtss2sd " << destination << ", " << src << "\n";
}

void Assembly::cvtsd2ss(const Operand &destination, const Operand &src) {
    _output << "\tcvtsd2ss " << destination << ", " << src << "\n";
}

void Assembly::cvtsi2ss(const Operand &destination, const Operand &src) {
    _output << "\tcvtsi2ss " << destination << ", " << src << "\n";
}

void Assembly::cvtsi2sd(const Operand &destination, const Operand &src) {
    _output << "\tcvtsi2sd " << destination << ", " << src << "\n";
}

void Assembly::movsx(const Operand &destination, const Operand &src) {
    _output << "\tmovsx " << destination << ", " << src << "\n";
}

void Assembly::movzx(const Operand &destination, const Operand &src) {
    _output << "\tmovzx " << destination << ", " << src << "\n";
}

void Assembly::movsxd(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovsxd " << destination << ", " << src << "\n";
}

void Assembly::movss(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovss " << destination << ", " << src << "\n";
}

void Assembly::movsd(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovsd " << destination << ", " << src << "\n";
}

void Assembly::mov(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmov " << destination << ", " << src << "\n";
}

void Assembly::label(const std::string &name, bool newline) {
    _output << name << ": ";
    if (newline) _output << "\n";
}

void Assembly::label(const SharedSymbol &symbol) {
    _output << symbol << ":\n";
}

void Assembly::jmp(const SharedSymbol &destination) {
    _output << "\tjmp " << destination << "\n";
}

void Assembly::jne(const SharedSymbol &destination) {
    _output << "\tjne " << destination << "\n";
}

void Assembly::pop(const Operand &destination) {
    _output << "\tpop " << destination << "\n";
}

void Assembly::push(const Operand &src) {
    _output << "\tpush " << src << "\n";
}

void Assembly::ret() {
    _output << "\tret\n";
}

void Assembly::ucomiss(const Operand &destination, const Operand &source) {
    _output << "\tucomiss " << destination << ", " << source << "\n";
}

void Assembly::ucomisd(const Operand &destination, const Operand &source) {
    _output << "\tucomisd " << destination << ", " << source << "\n";
}

void Assembly::pxor(const Operand &destination, const Operand &source) {
    _output << "\tpxor " << destination << ", " << source << "\n";
}

void Assembly::cmp(const Operand &first, const Operand &second) {
    _output << "\tcmp " << first << ", " << second << "\n";
}

void Assembly::setne(const Operand &destination) {
    _output << "\tsetne " << destination << "\n";
}

void Assembly::add(const Operand &destination, const Operand &src) {
    _output << "\tadd " << destination << ", " << src << "\n";
}

void Assembly::addsd(const Operand &destination, const Operand &src) {
    _output << "\taddsd " << destination << ", " << src << "\n";
}

void Assembly::addss(const Operand &destination, const Operand &src) {
    _output << "\taddss " << destination << ", " << src << "\n";
}

void Assembly::sub(const Operand &destination, const Operand &src) {
    _output << "\tsub " << destination << ", " << src << "\n";
}

void Assembly::subsd(const Operand &destination, const Operand &src) {
    _output << "\tsubsd " << destination << ", " << src << "\n";
}

void Assembly::subss(const Operand &destination, const Operand &src) {
    _output << "\tsubss " << destination << ", " << src << "\n";
}

void Assembly::idiv(const Operand &dividend) {
    _output << "\tidiv " << dividend << "\n";
}

void Assembly::div(const Operand &dividend) {
    _output << "\tdiv " << dividend << "\n";
}

void Assembly::divsd(const Operand &destination, const Operand &src) {
    _output << "\tdivsd " << destination << ", " << src << "\n";
}

void Assembly::divss(const Operand &destination, const Operand &src) {
    _output << "\tdivss " << destination << ", " << src << "\n";
}

void Assembly::imul(const Operand &destination, const Operand &src) {
    _output << "\timul " << destination << ", " << src << "\n";
}

void Assembly::mulsd(const Operand &destination, const Operand &src) {
    _output << "\tmulsd " << destination << ", " << src << "\n";
}

void Assembly::mulss(const Operand &destination, const Operand &src) {
    _output << "\tmulss " << destination << ", " << src << "\n";
}

void Assembly::directive(const std::string &name, const std::vector<std::string> &settings) {
    _output << name;

    for (size_t index = 0; index < settings.size(); index++) {
        const auto &setting = settings[index];
        _output << " " << setting;
        if (index != settings.size() - 1) _output << ",";
    }

    _output << "\n";
}

void Assembly::call(const std::string &name) {
    _output << "\tcall " << name << "\n";
}

void Assembly::call(const SharedSymbol &symbol) {
    _output << "\tcall " << symbol << "\n";
}

void Assembly::syscall() {
    _output << "\tsyscall\n";
}

void Assembly::comment(const std::string &comment) {
    _output << "\t# " << comment;
}

void Assembly::newline() {
    _output << "\n";
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
