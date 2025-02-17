#pragma once

#include "utils/visitor.hpp"
#include "mid/il_printer.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"

namespace arkoi::mid {

class CFGPrinter : Visitor {
public:
    CFGPrinter(std::stringstream &output) : _output(output), _printer(output) {};

public:
    [[nodiscard]] static std::stringstream print(Module &module);

    void visit(Module &module) override;

    void visit(Function &function) override;

    void visit(BasicBlock &block) override;

    void visit(Label &instruction) override { _printer.visit(instruction); }

    void visit(Return &instruction) override { _printer.visit(instruction); }

    void visit(Binary &instruction) override { _printer.visit(instruction); }

    void visit(Cast &instruction) override { _printer.visit(instruction); }

    void visit(Call &instruction) override { _printer.visit(instruction); }

    void visit(Goto &instruction) override { _printer.visit(instruction); }

    void visit(If &instruction) override { _printer.visit(instruction); }

    void visit(Alloca &instruction) override { _printer.visit(instruction); }

    void visit(Store &instruction) override { _printer.visit(instruction); }

    void visit(Load &instruction) override { _printer.visit(instruction); }

    void visit(Constant &instruction) override { _printer.visit(instruction); }

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream &_output;
    ILPrinter _printer;
};

} // namespace arkoi::mid

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