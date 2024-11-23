#include "def/symbol.hpp"

using namespace arkoi::symbol;

std::ostream &operator<<(std::ostream &os, const SharedSymbol &symbol) {
    std::visit([&os](const auto &symbol) { os << symbol.name(); }, *symbol);
    return os;
}