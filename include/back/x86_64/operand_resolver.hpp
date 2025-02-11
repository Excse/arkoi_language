#pragma once

#include "back/x86_64/operand.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"

namespace arkoi::back::x86_64 {

struct ConstantData {
    x86_64::Operand operand;
    std::string name;
};

class OperandResolver : mid::Visitor {
public:
    OperandResolver() : _parameter_offset(8) {}

    [[nodiscard]] static OperandResolver resolve(mid::Function &function);

    [[nodiscard]] x86_64::Operand resolve_operand(const mid::Operand &operand);

    [[nodiscard]] int64_t local_size() const { return _local_size; }

    [[nodiscard]] auto &constants() const { return _constants; }

private:
    void visit(mid::Begin &instruction) override;

    void visit(mid::Label &) override {};

    void visit(mid::Return &instruction) override;

    void visit(mid::Binary &instruction) override;

    void visit(mid::Cast &instruction) override;

    void visit(mid::Call &instruction) override;

    void visit(mid::If &instruction) override;

    void visit(mid::Store &instruction) override;

    void visit(mid::Goto &) override {};

    void visit(mid::End &instruction) override;

    [[nodiscard]] x86_64::Operand _resolve_variable(const mid::Variable &variable);

    [[nodiscard]] x86_64::Operand _resolve_constant(const mid::Constant &constant);

    [[nodiscard]] x86_64::Operand _resolve_temporary(const symbol::Temporary &symbol);

    [[nodiscard]] x86_64::Operand _resolve_parameter(const mid::Variable &variable,
                                                     size_t &int_index,
                                                     size_t &sse_index);

    [[nodiscard]] static std::optional<Register> _resolve_parameter_register(const symbol::Parameter &symbol,
                                                                             size_t &int_index,
                                                                             size_t &sse_index);

private:
    std::unordered_map<mid::Variable, x86_64::Operand> _resolved{};
    std::unordered_map<mid::Constant, ConstantData> _constants{};
    int64_t _parameter_offset;
    int64_t _local_size{};
};

} // namespace arkoi::back::x86_64

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
