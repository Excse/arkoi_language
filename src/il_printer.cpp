#include "il_printer.h"

#include <iostream>
#include <variant>
#include <cassert>

#include "symbol_table.h"
#include "instruction.h"

void ILPrinter::visit(const LabelInstruction &node) {
    std::cout << "LABEL " << *node.symbol() << ": " << std::endl;
}

void ILPrinter::visit(const BeginInstruction &) {
    std::cout << "BEGIN" << std::endl;
}

void ILPrinter::visit(const ReturnInstruction &node) {
    std::cout << "RETURN " << node.value() << std::endl;
}

void ILPrinter::visit(const BinaryInstruction &node) {
    assert(std::holds_alternative<std::shared_ptr<Symbol>>(node.result()));

    auto op = BinaryInstruction::type_to_string(node.type());
    std::cout << node.result() << " = " << node.left() << " " << op << " " << node.right() << std::endl;
}

void ILPrinter::visit(const EndInstruction &) {
    std::cout << "END" << std::endl;
}
