#include "mid/instruction.hpp"

using namespace arkoi::mid;

Binary::Operator Binary::node_to_instruction(ast::Binary::Operator op) {
    switch (op) {
        case ast::Binary::Operator::Add: return Operator::Add;
        case ast::Binary::Operator::Sub: return Operator::Sub;
        case ast::Binary::Operator::Mul: return Operator::Mul;
        case ast::Binary::Operator::Div: return Operator::Div;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool Store::has_side_effects() const {
    return !std::holds_alternative<symbol::Temporary>(*_result.symbol());
}

void InstructionType::accept(mid::Visitor &visitor) {
    std::visit([&](auto &item) { item.accept(visitor); }, *this);
}

std::ostream &operator<<(std::ostream &os, const Binary::Operator &op) {
    switch (op) {
        case Binary::Operator::Add: return os << "ADD";
        case Binary::Operator::Sub: return os << "SUB";
        case Binary::Operator::Mul: return os << "MUL";
        case Binary::Operator::Div: return os << "DIV";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
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
