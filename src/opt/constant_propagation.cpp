#include "opt/constant_propagation.hpp"

#include <ranges>

#include "utils/utils.hpp"

using namespace arkoi::opt;

bool ConstantPropagation::new_function(mid::Function &) {
    _constants.clear();
    return false;
}

bool ConstantPropagation::new_block(mid::BasicBlock &block) {
    auto has_changed = false;

    for (auto &instruction: block.instructions()) {
        _add_constant(instruction);

        has_changed |= _can_propagate(instruction);
    }

    return has_changed;
}

bool ConstantPropagation::_propagate(mid::Operand &operand) {
    auto condition_symbol = std::get_if<mid::Variable>(&operand);
    if (condition_symbol == nullptr) return false;

    auto result = _constants.find(*condition_symbol);
    if (result == _constants.end()) return false;

    operand = result->second;
    return true;
}

void ConstantPropagation::_add_constant(mid::InstructionType &type) {
    if (!std::holds_alternative<mid::Store>(type)) return;

    auto &store = std::get<mid::Store>(type);
    if (store.has_side_effects()) return;

    auto *constant = std::get_if<mid::Constant>(&store.value());
    if (constant == nullptr) return;

    _constants[store.result()] = *constant;
}

bool ConstantPropagation::_can_propagate(mid::InstructionType &type) {
    auto propagated = false;

    std::visit(match{
        [&](mid::Binary &instruction) {
            propagated |= _propagate(instruction.left());
            propagated |= _propagate(instruction.right());
        },
        [&](mid::Return &instruction) {
            propagated |= _propagate(instruction.value());
        },
        [&](mid::Cast &instruction) {
            propagated |= _propagate(instruction.expression());
        },
        [&](mid::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                propagated |= _propagate(argument);
            }
        },
        [&](mid::If &instruction) {
            propagated |= _propagate(instruction.condition());
        },
        [](const auto &) {}
    }, type);

    return propagated;
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
