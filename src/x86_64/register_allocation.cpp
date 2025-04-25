#include "x86_64/register_allocation.hpp"

#include <ranges>
#include <array>

using namespace arkoi::x86_64;

static constexpr std::array INTEGER_REGISTERS{
    Register::Base::B, Register::Base::R12, Register::Base::R13, Register::Base::R14,
    Register::Base::R15,
};

static constexpr std::array FLOATING_REGISTERS{
    Register::Base::XMM8, Register::Base::XMM9, Register::Base::XMM12, Register::Base::XMM13,
    Register::Base::XMM14, Register::Base::XMM15,
};

RegisterAllocater::RegisterAllocater(il::Function &function) : _function(function) {
    _renumber();
    _build();
    _simplify();
}

void RegisterAllocater::_renumber() {
    _analysis = il::DataflowAnalysis<il::InstructionLivenessAnalysis>();
    _analysis.run(_function);
}

void RegisterAllocater::_build() {
    _graph = InterferenceGraph<il::Variable>();

    for (const auto &in_operands: std::views::values(_analysis.in())) {
        for (const auto &operand: in_operands) {
            const auto *op_variable = std::get_if<il::Variable>(&operand);
            if (!op_variable) continue;
            _graph.add_node(*op_variable);
        }
    }

    for (const auto &[instruction, out_operands]: _analysis.out()) {
        for (const auto &definition: instruction->defs()) {
            const auto *def_variable = std::get_if<il::Variable>(&definition);
            if (!def_variable) continue;

            for (const auto &operand: out_operands) {
                const auto *op_variable = std::get_if<il::Variable>(&operand);
                if (!op_variable) continue;

                _graph.add_edge(*def_variable, *op_variable);
            }
        }
    }
}

void RegisterAllocater::_simplify() {
    std::stack<il::Variable> stack;

    auto work_list = _graph.nodes();
    while (!work_list.empty()) {
        auto found = false;

        for (const auto &node: work_list) {
            const auto interferences = _graph.interferences(node);

            const auto is_floating = std::holds_alternative<sem::Floating>(node.type());
            if (is_floating && interferences.size() >= FLOATING_REGISTERS.size()) continue;
            if (!is_floating && interferences.size() >= INTEGER_REGISTERS.size()) continue;

            stack.push(node);
            work_list.erase(node);
            found = true;
            break;
        }

        if (found) continue;

        const auto &first = *work_list.begin();
        _spilled.push_back(first);
        work_list.erase(first);
    }

    while (!stack.empty()) {
        auto node = stack.top();
        stack.pop();

        std::unordered_set<Register::Base> colors;
        for (const auto &interference: _graph.interferences(node)) {
            const auto found = _assigned.find(interference);
            if (found == _assigned.end()) continue;
            colors.insert(found->second);
        }

        auto found = false;
        if (std::holds_alternative<sem::Floating>(node.type())) {
            for (const auto base: FLOATING_REGISTERS) {
                if (colors.contains(base)) continue;
                _assigned[node] = base;
                found = true;
                break;
            }
        } else {
            for (const auto base: INTEGER_REGISTERS) {
                if (colors.contains(base)) continue;
                _assigned[node] = base;
                found = true;
                break;
            }
        }

        if (found) continue;

        _spilled.push_back(node);
    }
}

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
