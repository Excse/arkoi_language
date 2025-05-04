#pragma once

#include <sstream>

#include "il/instruction.hpp"
#include "x86_64/assembly.hpp"
#include "x86_64/mapper.hpp"

namespace arkoi::x86_64 {

struct ClassifiedArguments {
    std::vector<il::Operand> floating{};
    std::vector<il::Operand> integer{};
    std::vector<il::Operand> stack{};
};

class Generator final : il::Visitor {
public:
    explicit Generator(il::Module &module);

    [[nodiscard]] std::stringstream output() const;

private:
    void visit(il::Module &module) override;

    void visit(il::Function &function) override;

    void visit(il::BasicBlock &block) override;

    void visit(il::Return &) override {}

    void visit(il::Binary &instruction) override;

    void _add(const Operand &result, Operand left, const Operand &right, const sem::Type &type);

    void _sub(const Operand &result, Operand left, const Operand &right, const sem::Type &type);

    void _mul(const Operand &result, Operand left, const Operand &right, const sem::Type &type);

    void _div(const Operand &result, Operand left, Operand right, const sem::Type &type);

    void _gth(const Operand &result, Operand left, const Operand &right, const sem::Type &type);

    void _lth(const Operand &result, Operand left, const Operand &right, const sem::Type &type);

    void visit(il::Cast &instruction) override;

    void _float_to_float(const Operand &result, Operand source, const sem::Floating &from, const sem::Floating &to);

    void _int_to_int(const Operand &result, Operand source, const sem::Integral &from, const sem::Integral &to);

    void _float_to_int(const Operand &result, const Operand& source, const sem::Floating &from, const sem::Integral &to);

    void _float_to_bool(const Operand &result, const Operand& source, const sem::Floating &from, const sem::Boolean &to);

    void _int_to_float(const Operand &result, Operand source, const sem::Integral &from, const sem::Floating &to);

    void _int_to_bool(const Operand &result, Operand source, const sem::Integral &from, const sem::Boolean &to);

    void _bool_to_float(const Operand &result, Operand source, const sem::Boolean &from, const sem::Floating &to);

    void _bool_to_int(const Operand &result, Operand source, const sem::Boolean &from, const sem::Integral &to);

    void visit(il::Call &instruction) override;

    size_t _generate_arguments(const std::vector<il::Operand> &arguments);

    void visit(il::If &instruction) override;

    void visit(il::Goto &instruction) override;

    void visit(il::Alloca &) override {}

    void visit(il::Store &instruction) override;

    void visit(il::Load &instruction) override;

    void visit(il::Constant &instruction) override;

    Operand _load(const il::Operand &operand);

    void _store(Operand source, const Operand &destination, const sem::Type &type);

    Register _adjust_to_reg(const Operand &result, const Operand &target, const sem::Type &type);

    [[nodiscard]] Register _store_temp_1(const Operand &source, const sem::Type &type);

    [[nodiscard]] static Register _temp_1_register(const sem::Type &type);

    [[nodiscard]] Register _store_temp_2(const Operand &source, const sem::Type &type);

    [[nodiscard]] static Register _temp_2_register(const sem::Type &type);

    static void _directive(const std::string &directive, std::vector<AssemblyItem> &output);

    void _label(const std::string &name);

    void _jmp(const std::string &name);

    void _jnz(const std::string &name);

    void _call(const std::string &name);

    void _movsxd(const Operand &destination, const Operand &source);

    void _movsd(const Operand &destination, const Operand &source);

    void _movss(const Operand &destination, const Operand &source);

    void _movzx(const Operand &destination, const Operand &source);

    void _movsx(const Operand &destination, const Operand &source);

    void _mov(const Operand &destination, const Operand &source);

    void _addsd(const Operand &destination, const Operand &source);

    void _addss(const Operand &destination, const Operand &source);

    void _add(const Operand &destination, const Operand &source);

    void _subsd(const Operand &destination, const Operand &source);

    void _subss(const Operand &destination, const Operand &source);

    void _sub(const Operand &destination, const Operand &source);

    void _mulsd(const Operand &destination, const Operand &source);

    void _mulss(const Operand &destination, const Operand &source);

    void _imul(const Operand &destination, const Operand &source);

    void _divsd(const Operand &destination, const Operand &source);

    void _divss(const Operand &destination, const Operand &source);

    void _idiv(const Operand &source);

    void _udiv(const Operand &source);

    void _xorps(const Operand &destination, const Operand &source);

    void _or(const Operand &destination, const Operand &source);

    void _ucomisd(const Operand &destination, const Operand &source);

    void _ucomiss(const Operand &destination, const Operand &source);

    void _cvtsd2ss(const Operand &destination, const Operand &source);

    void _cvtss2sd(const Operand &destination, const Operand &source);

    void _cvtsi2sd(const Operand &destination, const Operand &source);

    void _cvtsi2ss(const Operand &destination, const Operand &source);

    void _cvttsd2si(const Operand &destination, const Operand &source);

    void _cvttss2si(const Operand &destination, const Operand &source);

    void _test(const Operand &first, const Operand &second);

    void _cmp(const Operand &first, const Operand &second);

    void _setne(const Operand &destination);

    void _setg(const Operand &destination);

    void _seta(const Operand &destination);

    void _setb(const Operand &destination);

    void _setl(const Operand &destination);

    void _setp(const Operand &destination);

    void _enter(uint16_t size, uint8_t nesting_level);

    void _syscall();

    void _leave();

    void _ret();

    static void _newline(std::vector<AssemblyItem> &output);

private:
    std::unique_ptr<Mapper> _current_mapper{};
    std::vector<AssemblyItem> _data{};
    std::vector<AssemblyItem> _text{};
    size_t _constants{};
    il::Module &_module;
};

} // namespace arkoi::x86_64

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
