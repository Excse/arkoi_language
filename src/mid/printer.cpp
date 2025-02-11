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
            << instruction.type() << " "
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

void Printer::visit(Store &instruction) {
    _output << "  ";
    _output << instruction.result() << " = " << instruction.value() << "\n";
}
