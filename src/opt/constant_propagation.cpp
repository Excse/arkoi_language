#include "opt/constant_propagation.hpp"

#include <unordered_map>

#include "il/operand.hpp"
#include "utils/utils.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool ConstantPropagation::on_block(il::BasicBlock &block) {
    bool changed = false;

    _constants.clear();

    for (auto &instruction: block.instructions()) {
        if(auto *constant = std::get_if<il::Constant>(&instruction)) {
            _constants[constant->result()] = constant->immediate();
        }

        changed |= _can_propagate(instruction);
    }

    return changed;
}

bool ConstantPropagation::_can_propagate(il::Instruction &instruction) {
    auto propagated = false;

    std::visit(match{
        [&](il::Binary &instruction) {
            propagated |= _propagate(instruction.left());
            propagated |= _propagate(instruction.right());
        },
        [&](il::Return &instruction) {
            propagated |= _propagate(instruction.value());
        },
        [&](il::Cast &instruction) {
            propagated |= _propagate(instruction.source());
        },
        [&](il::If &instruction) {
            propagated |= _propagate(instruction.condition());
        },
        [&](il::Store &instruction) {
            propagated |= _propagate(instruction.source());
        },
        [&](il::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                propagated |= _propagate(argument);
            }
        },
        [&](il::Constant &) {},
        [&](il::Alloca &) {},
        [&](il::Load &) {},
        [&](il::Goto &) {},
    }, instruction);

    return propagated;
}

bool ConstantPropagation::_propagate(il::Operand &operand) {
    const auto *variable = std::get_if<il::Variable>(&operand);
    if (variable == nullptr) return false;

    const auto result = _constants.find(*variable);
    if (result == _constants.end()) return false;

    operand = result->second;
    return true;
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