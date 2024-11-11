#include "semantic/symbol.hpp"

std::ostream &operator<<(std::ostream &os, const SharedSymbol &symbol) {
    std::visit([&os](const auto &symbol) { os << symbol.name(); }, *symbol);
    return os;
}