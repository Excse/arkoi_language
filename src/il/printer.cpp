#include "il/printer.hpp"

#include <iostream>

#include "semantic/symbol_table.hpp"
#include "il/instruction.hpp"
#include "utils/utils.hpp"

namespace il {

Printer Printer::print(std::vector<CFG> &cfgs) {
    Printer printer;

    for (auto &cfg: cfgs) {
        cfg.linearize([&](auto &instruction) {
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
    _output << "BEGIN " << instruction.label() << "\n";
}

void Printer::visit(Return &instruction) {
    _output << "RETURN " << instruction.value() << "\n";
}

void Printer::visit(Binary &instruction) {
    _output << instruction.result() << " = " << to_string(instruction.op()) << " " << instruction.left() << ", "
            << instruction.right() << "\n";
}

void Printer::visit(Cast &instruction) {
    _output << instruction.result() << " = CAST " << instruction.expression() << " FROM @" << instruction.from()
            << " TO @" << instruction.to() << "\n";
}

void Printer::visit(End &) {
    _output << "END\n";
}

void Printer::visit(Call &instruction) {
    _output << instruction.result() << " = CALL " << instruction.symbol() << "\n";
}

void Printer::visit(Argument &instruction) {
    _output << "ARG " << instruction.result() << " = " << instruction.expression() << "\n";
}

void Printer::visit(Goto &instruction) {
    _output << "GOTO " << instruction.label() << "\n";
}

void Printer::visit(IfNot &instruction) {
    _output << "IF NOT " << instruction.condition() << " GOTO " << instruction.label() << "\n";
}

void Printer::visit(Store &instruction) {
    _output << instruction.result() << " = " << instruction.value() << "\n";
}

}
