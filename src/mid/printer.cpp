#include "mid/printer.hpp"

#include <iostream>

#include "mid/symbol_table.hpp"
#include "mid/instruction.hpp"
#include "utils/utils.hpp"

using namespace arkoi::mid;

Printer Printer::print(Module &module) {
    Printer printer;
    printer.visit(module);
    return printer;
}

void Printer::visit(Module &module) {
    for (auto &function: module.functions()) {
        function.accept(*this);
    }
}

void Printer::visit(Function &function) {
    auto &symbol = std::get<symbol::Function>(*function.symbol());
    _output << "FUN " << symbol.name() << "(";

    for (size_t index = 0; index < symbol.parameter_symbols().size(); index++) {
        auto &parameter = std::get<symbol::Parameter>(*symbol.parameter_symbols()[index]);
        _output << parameter.name() << " @" << parameter.type().value();

        if (index != symbol.parameter_symbols().size() - 1) {
            _output << ", ";
        }
    }

    _output << ") @" << symbol.return_type().value() << "\n";

    function.depth_first_search([&](BasicBlock &block) {
        block.accept(*this);
    });

    _output << "\n";
}

void Printer::visit(BasicBlock &block) {
    for (auto &instruction: block.instructions()) {
        instruction.accept(*this);
    }
}

void Printer::visit(Label &instruction) {
    _output << "LABEL " << instruction.symbol() << ":\n";
}

void Printer::visit(Return &instruction) {
    _output << "  ";
    _output << "RETURN " << instruction.value() << "\n";
}

void Printer::visit(Binary &instruction) {
    _output << "  ";
    _output << instruction.result() << " = " << to_string(instruction.op()) << " @"
            << instruction.op_type() << " "
            << instruction.left() << ", " << instruction.right() << "\n";
}

void Printer::visit(Cast &instruction) {
    _output << "  ";
    _output << instruction.result() << " = CAST " << instruction.expression() << " @" << instruction.from()
            << " TO @" << instruction.to() << "\n";
}

void Printer::visit(Call &instruction) {
    _output << "  ";
    _output << instruction.result() << " = CALL " << instruction.function() << "(";

    for (size_t index = 0; index < instruction.arguments().size(); index++) {
        auto &argument = instruction.arguments()[index];
        _output << argument;

        if (index != instruction.arguments().size() - 1) {
            _output << ", ";
        }
    }

    _output << ")\n";
}

void Printer::visit(Goto &instruction) {
    _output << "  ";
    _output << "GOTO " << instruction.label() << "\n";
}

void Printer::visit(If &instruction) {
    _output << "  ";
    _output << "IF " << instruction.condition() << " GOTO " << instruction.label() << "\n";
}

void Printer::visit(Alloca &instruction) {
    _output << "  ";
    _output << instruction.result() << " = ALLOCA @" << instruction.type() << "\n";
}

void Printer::visit(Store &instruction) {
    _output << "  ";
    _output << "STORE @" << instruction.type() << " " << instruction.value()
            << " IN " << instruction.result() << "\n";
}

void Printer::visit(Load &instruction) {
    _output << "  ";
    _output << instruction.result() << " = LOAD @" << instruction.type()
            << " " << instruction.target() << "\n";
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
