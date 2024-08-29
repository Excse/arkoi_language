#include "symbol.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const Symbol &symbol) {
    std::visit([&](const auto &symbol) {
        os << symbol.name();
    }, symbol);
    return os;
}