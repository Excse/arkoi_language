#pragma once

#include "utils/interference_graph.hpp"
#include "il/instruction.hpp"
#include "il/analyses.hpp"
#include "x86_64/operand.hpp"

namespace arkoi::x86_64 {
class RegisterAllocater {
public:
    using Mapping = std::unordered_map<il::Variable, Register::Base>;

public:
    RegisterAllocater(il::Function &function, Mapping precolored);

    [[nodiscard]] auto &assigned() { return _assigned; }

    [[nodiscard]] auto &spilled() { return _spilled; }

private:
    void _renumber();

    void _build();

    void _simplify();

private:
    il::DataflowAnalysis<il::InstructionLivenessAnalysis> _analysis;
    InterferenceGraph<il::Variable> _graph;
    std::vector<il::Variable> _spilled;
    il::Function &_function;
    Mapping _assigned;
};
} // namespace arkoi::x86_64

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
