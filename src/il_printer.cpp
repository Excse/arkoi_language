#include "il_printer.h"

#include <iostream>
#include <variant>

#include "symbol_table.h"
#include "utils.h"
#include "tac.h"

void ILPrinter::visit(const LabelInstruction &node) {
    std::cout << "LABEL " << node.name() << ": " << std::endl;
}

void ILPrinter::visit(const ReturnInstruction &node) {
    std::cout << "RETURN ";

    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { std::cout << *symbol; },
            [&](const auto &item) { std::cout << item; }
    }, node.value());

    std::cout << std::endl;
}
