#include "mid/cfg_printer.hpp"

using namespace arkoi::mid;

std::stringstream CFGPrinter::print(Module &module) {
    std::stringstream output;
    CFGPrinter printer(output);
    printer.visit(module);
    return output;
}

void CFGPrinter::visit(Module &module) {
    _output << "digraph CFG {\n";

    _output << "node [shape=box, labelloc=\"t\", labeljust=\"l\"]\n";

    for (auto &function: module.functions()) {
        function.accept(*this);
    }

    _output << "}\n";
}

void CFGPrinter::visit(Function &function) {
    function.depth_first_search([this](BasicBlock &block) { block.accept(*this); });
}

void CFGPrinter::visit(BasicBlock &block) {
    _output << block.symbol() << " [label=\"";

    for (auto &instruction: block.instructions()) {
        instruction.accept(*this);
        _output << "\\l";
    }

    _output << "\"]\n";

    if(block.next()) {
        _output << block.symbol() << " -> " << block.next()->symbol() << " [label=\"Next\"]\n";
    }

    if(block.branch()) {
        _output << block.symbol() << " -> " << block.branch()->symbol() << " [label=\"Branch\"]\n";
    }
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
