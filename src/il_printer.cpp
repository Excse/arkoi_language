#include "il_printer.h"

#include <iostream>
#include <variant>
#include <cassert>

#include "symbol_table.h"
#include "instruction.h"

void ILPrinter::visit(LabelInstruction &node) {
    std::cout << "LABEL " << *node.symbol() << ": " << std::endl;
}

void ILPrinter::visit(BeginInstruction &) {
    std::cout << "BEGIN" << std::endl;
}

void ILPrinter::visit(ReturnInstruction &node) {
    std::cout << "RETURN " << node.value() << std::endl;
}

void ILPrinter::visit(BinaryInstruction &node) {
    assert(std::holds_alternative<std::shared_ptr<Symbol>>(node.result()));

    auto op = BinaryInstruction::type_to_string(node.type());
    std::cout << node.result() << " = " << node.left() << " " << op << " " << node.right() << std::endl;
}

void ILPrinter::visit(EndInstruction &) {
    std::cout << "END" << std::endl;
}
