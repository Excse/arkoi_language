#include "opt/constant_folding.hpp"

#include "utils/utils.hpp"
#include "il/operand.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool ConstantFolding::on_block(il::BasicBlock &block) {
    bool changed = false;

    for (auto &instruction: block.instructions()) {
        if(!instruction.is_constant()) continue;

        if (auto cast = std::get_if<il::Cast>(&instruction)) {
            auto value = _cast(*cast);
            instruction = il::Constant(cast->result(), value);
            changed = true;
        }
    }

    return changed;
}

il::Immediate ConstantFolding::_cast(il::Cast &instruction) {
    auto expression = std::get<il::Immediate>(instruction.expression());

    return std::visit([&](const auto &value) {
        return _evaluate_cast(instruction.result().type(), value);
    }, expression);
}

il::Immediate ConstantFolding::_evaluate_cast(const sem::Type &to, auto expression) {
    return std::visit(match{
        [&](const sem::Integral &type) -> il::Immediate {
            switch (type.size()) {
                case Size::BYTE: return type.sign() ? (int8_t) expression : (uint8_t) expression;
                case Size::WORD: return type.sign() ? (int16_t) expression : (uint16_t) expression;
                case Size::DWORD: return type.sign() ? (int32_t) expression : (uint32_t) expression;
                case Size::QWORD: return type.sign() ? (int64_t) expression : (uint64_t) expression;
                default: std::unreachable();
            }
        },
        [&](const sem::Floating &type) -> il::Immediate {
            switch (type.size()) {
                case Size::DWORD: return (float) expression;
                case Size::QWORD: return (double) expression;
                default: std::unreachable();
            }
        },
        [&](const sem::Boolean &) -> il::Immediate {
            return (bool) expression;
        }
    }, to);
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