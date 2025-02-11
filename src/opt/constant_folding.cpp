#include "opt/constant_folding.hpp"

#include "utils/utils.hpp"
#include "mid/operand.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool ConstantFolding::new_block(mid::BasicBlock &block) {
    auto has_changed = false;

    for (auto &instruction: block.instructions()) {
        if (auto binary = std::get_if<mid::Binary>(&instruction)) {
            auto result = _binary(*binary);
            if (!result.has_value()) continue;

            instruction = *result;
            has_changed = true;
        } else if (auto cast = std::get_if<mid::Cast>(&instruction)) {
            auto result = _cast(*cast);
            if (!result.has_value()) continue;

            instruction = *result;
            has_changed = true;
        }
    }

    return has_changed;
}

std::optional<mid::InstructionType> ConstantFolding::_binary(const mid::Binary &instruction) {
    auto *right_constant = std::get_if<mid::Constant>(&instruction.right());
    auto *left_constant = std::get_if<mid::Constant>(&instruction.left());

    if (!right_constant || !left_constant) return std::nullopt;

    auto value = std::visit([&](const auto &left, const auto &right) {
        return _evaluate_binary(instruction, left, right);
    }, *right_constant, *left_constant);

    return mid::Store(instruction.result(), value, instruction.type());
}

mid::Operand ConstantFolding::_evaluate_binary(const mid::Binary &instruction, auto left, auto right) {
    switch (instruction.op()) {
        case mid::Binary::Operator::Add: return left + right;
        case mid::Binary::Operator::Sub: return left - right;
        case mid::Binary::Operator::Mul: return left * right;
        case mid::Binary::Operator::Div: return left / right;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::optional<mid::InstructionType> ConstantFolding::_cast(const mid::Cast &instruction) {
    auto *expression = std::get_if<mid::Constant>(&instruction.expression());

    if (!expression) return std::nullopt;

    auto value = std::visit([&](const auto &value) {
        return _evaluate_unary(instruction.to(), value);
    }, *expression);

    return mid::Store(instruction.result(), value, instruction.to());
}

mid::Operand ConstantFolding::_evaluate_unary(const Type &to, auto expression) {
    return std::visit(match{
        [&](const type::Integral &type) -> mid::Operand {
            switch (type.size()) {
                case Size::BYTE: return type.sign() ? (int8_t) expression : (uint8_t) expression;
                case Size::WORD: return type.sign() ? (int16_t) expression : (uint16_t) expression;
                case Size::DWORD: return type.sign() ? (int32_t) expression : (uint32_t) expression;
                case Size::QWORD: return type.sign() ? (int64_t) expression : (uint64_t) expression;
                default: std::unreachable();
            }
        },
        [&](const type::Floating &type) -> mid::Operand {
            switch (type.size()) {
                case Size::DWORD: return (float) expression;
                case Size::QWORD: return (double) expression;
                default: std::unreachable();
            }
        },
        [&](const type::Boolean &) -> mid::Operand {
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
