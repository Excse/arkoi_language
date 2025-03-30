#pragma once

#include <sstream>

#include "x86_64/mapper.hpp"
#include "il/instruction.hpp"
#include "il/il_printer.hpp"

namespace arkoi::x86_64 {

struct ClassifiedArguments {
    std::vector<il::Operand> floating{};
    std::vector<il::Operand> integer{};
    std::vector<il::Operand> stack{};
};

class Generator : il::Visitor {
public:
    Generator() : _text(), _printer(_text) {}

    [[nodiscard]] static std::stringstream generate(il::Module &module);

private:
    void visit(il::Module &module) override;

    void visit(il::Function &function) override;

    void visit(il::BasicBlock &block) override;

    void visit(il::Return &instruction) override;

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

    void _float_to_bool(const Operand &result, const Operand& source, const sem::Floating &from, const sem::Boolean &to);

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

    Register _adjust_to_reg(const Operand &result, const Operand &operand, const sem::Type &type);

    [[nodiscard]] Register _store_temp_1(const Operand &source, const sem::Type &type);

    [[nodiscard]] static Register _temp_1_register(const sem::Type &type);

    [[nodiscard]] Register _store_temp_2(const Operand &source, const sem::Type &type);

    [[nodiscard]] static Register _temp_2_register(const sem::Type &type);

private:
    il::Function *_current_function{};
    std::stringstream _data{};
    std::stringstream _text;
    il::ILPrinter _printer;
    size_t _constants{};
    Mapper _mapper{};
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
