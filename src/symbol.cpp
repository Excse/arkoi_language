#include "symbol.h"

std::ostream &operator<<(std::ostream &os, const Symbol &symbol) {
    std::visit([&os](const auto &symbol) { os << symbol.name(); }, symbol);
    return os;
}