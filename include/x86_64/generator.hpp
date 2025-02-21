#pragma once

#include <sstream>

#include "x86_64/mapper.hpp"
#include "il/instruction.hpp"

namespace arkoi::x86_64 {

class Generator : il::Visitor {
public:
    [[nodiscard]] static std::stringstream generate(il::Module &module);

private:
    void visit(il::Module &module) override;

    void visit(il::Function &function) override;

    void visit(il::BasicBlock &block) override;

    void visit(il::Return &instruction) override;

    void visit(il::Binary &instruction) override;

    void visit(il::Cast &instruction) override;

    void visit(il::Call &instruction) override;

    void visit(il::If &instruction) override;

    void visit(il::Goto &instruction) override;

    void visit(il::Alloca &) override {}

    void visit(il::Store &instruction) override;

    void visit(il::Load &instruction) override;

    void visit(il::Constant &instruction) override;

private:
    il::Function *_current_function;
    std::stringstream _text{};
    std::stringstream _data{};
    Mapper _mapper{};
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
