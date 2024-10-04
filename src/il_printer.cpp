#include "il_printer.hpp"

#include <iostream>

#include "symbol_table.hpp"
#include "instruction.hpp"
#include "utils.hpp"

ILPrinter ILPrinter::print(const std::vector<std::unique_ptr<Instruction>> &instructions) {
    ILPrinter printer;

    for (const auto &item: instructions) {
        item->accept(printer);
    }

    return printer;
}

ILPrinter ILPrinter::print(Instruction &instruction) {
    ILPrinter printer;

    instruction.accept(printer);

    return printer;
}

void ILPrinter::visit(LabelInstruction &instruction) {
    _output << "LABEL " << *instruction.symbol() << ":\n";
}

void ILPrinter::visit(BeginInstruction &instruction) {
    _output << "BEGIN";
    if (instruction.local_size()) _output << " " << instruction.local_size();
    _output << "\n";
}

void ILPrinter::visit(ReturnInstruction &instruction) {
    _output << "RETURN " << instruction.value() << "\n";
}

void ILPrinter::visit(BinaryInstruction &instruction) {
    _output << instruction.result() << " = " << to_string(instruction.op()) << " " << instruction.left() << ", "
            << instruction.right() << "\n";
}

void ILPrinter::visit(CastInstruction &instruction) {
    _output << instruction.result() << " = CAST " << instruction.expression() << " FROM @" << instruction.from()
            << " TO @" << instruction.to() << "\n";
}

void ILPrinter::visit(EndInstruction &) {
    _output << "END" << "\n";
}

void ILPrinter::visit(CallInstruction &instruction) {
    _output << instruction.result() << " = CALL " << *instruction.symbol() << "\n";
}

void ILPrinter::visit(ArgumentInstruction &instruction) {
    _output << "ARG " << instruction.expression() << "\n";
}