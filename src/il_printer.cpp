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

void ILPrinter::visit(const BinaryInstruction &node) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { std::cout << *symbol << " = "; },
            [&](const auto &item) {
                // TODO: Doesnt work!
                throw std::runtime_error("This shouldn't happen.");
            }
    }, node.result());

    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { std::cout << *symbol << " "; },
            [&](const auto &item) { std::cout << item << " "; }
    }, node.left());

    switch (node.type()) {
        case BinaryInstruction::Type::Add:
            std::cout << "+ ";
            break;
        case BinaryInstruction::Type::Sub:
            std::cout << "- ";
            break;
        case BinaryInstruction::Type::Div:
            std::cout << "/ ";
            break;
        case BinaryInstruction::Type::Mul:
            std::cout << "* ";
            break;
    }

    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { std::cout << *symbol; },
            [&](const auto &item) { std::cout << item; }
    }, node.right());

    std::cout << std::endl;
}
