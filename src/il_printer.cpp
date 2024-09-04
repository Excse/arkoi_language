#include "il_printer.h"

#include <iostream>

#include "symbol_table.h"
#include "instruction.h"
#include "utils.h"

void ILPrinter::visit(LabelInstruction &instruction) {
    _output << "LABEL " << *instruction.symbol() << ":\n";
}

void ILPrinter::visit(BeginInstruction &instruction) {
    _output << "BEGIN " << instruction.size() << "\n";
}

void ILPrinter::visit(ReturnInstruction &instruction) {
    _output << "RETURN " << instruction.value() << "\n";
}

void ILPrinter::visit(BinaryInstruction &instruction) {
    _output << instruction.result() << " = " << to_string(instruction.type()) << " " << instruction.left() << ", "
            << instruction.right() << "\n";
}

void ILPrinter::visit(EndInstruction &) {
    _output << "END" << "\n";
}

void ILPrinter::clear() {
    _output.str(std::string());
}
