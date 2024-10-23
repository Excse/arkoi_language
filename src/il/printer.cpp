#include "il/printer.hpp"

#include <iostream>

#include "semantic/symbol_table.hpp"
#include "il/instruction.hpp"
#include "utils/utils.hpp"

namespace il {

Printer Printer::print(std::vector<Function> &functions) {
    Printer printer;

    for (auto &function: functions) {
        function.linearize([&](auto &instruction) {
            instruction.accept(printer);
        });
    }

    return printer;
}

Printer Printer::print(Instruction &instruction) {
    Printer printer;

    instruction.accept(printer);

    return printer;
}

void Printer::visit(Label &instruction) {
    _output << "LABEL " << instruction.symbol() << ":\n";
}

void Printer::visit(Begin &instruction) {
    auto &function = std::get<FunctionSymbol>(*instruction.function());
    _output << "BEGIN " << function.name() << "(";

    for (size_t index = 0; index < function.parameter_symbols().size(); index++) {
        auto &parameter = std::get<ParameterSymbol>(*function.parameter_symbols()[index]);
        _output << parameter.name() << " @" << parameter.type().value();

        if (index != function.parameter_symbols().size() - 1) {
            _output << ", ";
        }
    }

    _output << ") @" << function.return_type().value() << "\n";
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

void Printer::visit(End &) {
    _output << "END\n";
}

void Printer::visit(Call &instruction) {
    _output << "  ";
    _output << instruction.result() << " = CALL " << instruction.symbol() << "(";

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

}
