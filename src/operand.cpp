#include "operand.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const FPRelative &operand) {
    if (operand.sign()) {
        os << "[rbp + " << to_string(operand.offset()) << "]";
    } else {
        os << "[rbp - " << to_string(operand.offset()) << "]";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { os << *symbol; },
            [&](const FPRelative &relative) { os << relative; },
            [&](const long long &value) { os << value; }
    }, operand);
    return os;
}