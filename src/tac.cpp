//
// Created by timo on 8/18/24.
//

#include "tac.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const TACLabel &instruction) {
    os << "LABEL " << instruction.name << ": ";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TACReturn &instruction) {
    os << "RETURN";

    if (instruction.value.has_value()) {
        os << " ";
        std::visit(match{
                [&](const std::shared_ptr<Symbol> &symbol) { os << *symbol; },
                [&](const auto &item) { os << item; }
        }, instruction.value.value());
    }

    return os;
}