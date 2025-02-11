#pragma once

#include "back/x86_64/operand_resolver.hpp"
#include "back/x86_64/assembly.hpp"
#include "back/x86_64/operand.hpp"
#include "mid/symbol_table.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"
#include "utils/visitor.hpp"

namespace arkoi::back::x86_64 {

class Generator : mid::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Generator generate(std::vector<mid::Function> &functions);

    void visit(mid::Label &instruction) override;

    void visit(mid::Begin &instruction) override;

    void visit(mid::Return &instruction) override;

    void visit(mid::Binary &memory) override;

    void visit(mid::Cast &instruction) override;

    void visit(mid::End &instruction) override;

    void visit(mid::Call &instruction) override;

    void visit(mid::Goto &instruction) override;

    void visit(mid::If &instruction) override;

    void visit(mid::Store &constant) override;

    [[nodiscard]] auto &output() const { return _assembly.output(); }

private:
    void _new_function(mid::Function &function);

    void _preamble();

    void _data_section();

    void _comment_instruction(mid::Instruction &instruction);

    void _convert_int_to_int(const mid::Cast &constant, const type::Integral &from,
                             const type::Integral &to);

    void _convert_int_to_float(const mid::Cast &constant, const type::Integral &from,
                               const type::Floating &to);

    void _convert_int_to_bool(const mid::Cast &instruction, const type::Integral &from,
                              const type::Boolean &to);

    void _convert_float_to_float(const mid::Cast &instruction, const type::Floating &from,
                                 const type::Floating &to);

    void _convert_float_to_int(const mid::Cast &instruction, const type::Floating &from,
                               const type::Integral &to);

    void _convert_float_to_bool(const mid::Cast &instruction, const type::Floating &from,
                                const type::Boolean &to);

    void _convert_bool_to_int(const mid::Cast &instruction, const type::Boolean &from,
                              const type::Integral &to);

    void _convert_bool_to_float(const mid::Cast &instruction, const type::Boolean &from,
                                const type::Floating &to);

    Operand _integer_promote(const type::Integral &type, const Operand &operand);

    void _mov(const Type &type, const Operand &destination, const Operand &src);

    void _add(const Type &type, const Operand &destination, const Operand &src);

    void _sub(const Type &type, const Operand &destination, const Operand &src);

    void _div(const Type &type, const Operand &destination, const Operand &src);

    void _mul(const Type &type, const Operand &destination, const Operand &src);

    [[nodiscard]] Register _move_to_temp1(const Type &type, const Operand &src);

    [[nodiscard]] Register _move_to_temp2(const Type &type, const Operand &src);

    [[nodiscard]] static Register _select_register(const Type &type, Register::Base integer, Register::Base floating);

    [[nodiscard]] static Register _returning_register(const Type &type);

    [[nodiscard]] static Register _temp1_register(const Type &type);

    [[nodiscard]] static Register _temp2_register(const Type &type);

private:
    std::unordered_map<mid::Constant, std::string> _constants{};
    OperandResolver _resolver;
    Assembly _assembly{};
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
