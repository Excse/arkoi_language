#include "il_printer.h"

#include <iostream>

#include "symbol_table.h"
#include "instruction.h"
#include "utils.h"

void ILPrinter::visit(LabelInstruction &node) {
    _output << "LABEL " << *node.symbol() << ":\n";
}

void ILPrinter::visit(BeginInstruction &node) {
    _output << "BEGIN " << node.size() << "\n";
}

void ILPrinter::visit(ReturnInstruction &node) {
    _output << "RETURN " << node.value() << "\n";
}

void ILPrinter::visit(BinaryInstruction &node) {
    _output << node.result() << " = " << node.left() << " " << to_string(node.type()) << " " << node.right() << "\n";
}

void ILPrinter::visit(EndInstruction &) {
    _output << "END" << "\n";
}

void ILPrinter::clear() {
    _output.str(std::string());
}
