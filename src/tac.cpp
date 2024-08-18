//
// Created by timo on 8/18/24.
//

#include "tac.h"

std::ostream &operator<<(std::ostream &os, const TACLabel &instruction) {
    os << "_" << instruction.name << ": ";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TACReturn &instruction) {
    os << "RETURN";

    if (instruction.value.has_value()) {
        os << " ";
        std::visit([&](const auto &value) { os << value; }, instruction.value.value());
    }

    return os;
}