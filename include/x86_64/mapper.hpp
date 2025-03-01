#pragma once

#include <unordered_map>
#include <set>

#include "x86_64/operand.hpp"
#include "il/instruction.hpp"

namespace arkoi::x86_64 {

class Mapper : il::Visitor {
public:
    [[nodiscard]] static Mapper map(il::Function &function);

    [[nodiscard]] Operand &operator[](const il::Variable& variable);

    [[nodiscard]] Operand operator[](const il::Operand &operand);

    [[nodiscard]] size_t stack_size() const;

    [[nodiscard]] static std::vector<il::Variable> get_int_parameters(const std::vector<il::Variable> &parameters);

    [[nodiscard]] static std::vector<il::Variable> get_see_parameters(const std::vector<il::Variable> &parameters);

    [[nodiscard]] static std::vector<il::Variable> get_stack_parameters(const std::vector<il::Variable> &parameters);

    [[nodiscard]] static size_t align_size(size_t input);

private:
    void visit(il::Module &) override {}

    void visit(il::Function &function) override;

    void visit(il::BasicBlock &block) override;

    void visit(il::Binary &instruction) override;

    void visit(il::Cast &instruction) override;

    void visit(il::Return &instruction) override;

    void visit(il::Call &instruction) override;

    Register _return_register(const sem::Type &type);

    void visit(il::If &) override {}

    void visit(il::Goto &) override {}

    void visit(il::Alloca &instruction) override;

    void visit(il::Store &) override {}

    void visit(il::Load &instruction) override;

    void visit(il::Constant &instruction) override;

    void _add_register(const il::Variable &variable, const Register &reg);

    void _add_memory(const il::Variable &variable, const Memory &memory);

    void _add_push(const il::Variable &variable);

private:
    std::unordered_map<il::Variable, Operand> _mappings{};
    std::set<il::Variable> _locals{};
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
