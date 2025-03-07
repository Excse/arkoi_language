#include "opt/dead_code_elimination.hpp"

#include <ranges>

#include "utils/utils.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool DeadCodeElimination::enter_function(il::Function &function) {
    _used.clear();

    for(auto &block : function) {
        for(auto &instruction : block) {
            std::visit(match{
                [&](il::Cast &instruction) {
                    _mark_variable(instruction.source());
                },
                [&](il::Return &instruction) {
                    _mark_variable(instruction.value());
                },
                [&](il::If &instruction) {
                    _mark_variable(instruction.condition());
                },
                [&](il::Store &instruction) {
                    _mark_variable(instruction.source());
                    _mark_variable(instruction.result());
                },
                [&](il::Load &instruction) {
                    _mark_variable(instruction.source());
                },
                [&](il::Binary &instruction) {
                    _mark_variable(instruction.left());
                    _mark_variable(instruction.right());
                },
                [&](il::Call &instruction) {
                    for (auto &argument: instruction.arguments()) {
                        _mark_variable(argument);
                    }
                },
                [&](il::Constant &) {},
                [&](il::Alloca &) {},
                [&](il::Goto &) {},
            }, instruction);
        }
    }

    return false;
}

bool DeadCodeElimination::on_block(il::BasicBlock &block) {
    return std::erase_if(block.instructions(), [&](auto &instruction) {
        return std::visit(match{
            [&](il::Binary &instruction) {
                return !_used.contains(instruction.result());
            },
            [&](il::Load &instruction) {
                return !_used.contains(instruction.result());
            },
            [&](il::Cast &instruction) {
                return !_used.contains(instruction.result());
            },
            [&](il::Constant &instruction) {
                return !_used.contains(instruction.result());
            },
            [&](il::Alloca &instruction) {
                return !_used.contains(instruction.result());
            },
            [&](il::Return &) { return false; },
            [&](il::Store &) { return false; },
            [&](il::Goto &) { return false; },
            [&](il::Call &) { return false; },
            [&](il::If &) { return false; },
        }, instruction);
    });
}

void DeadCodeElimination::_mark_variable(const il::Operand &operand) {
    const auto *variable = std::get_if<il::Variable>(&operand);
    if (variable == nullptr) return;

    _used.insert(*variable);
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
