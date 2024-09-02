#include "symbol.h"

std::ostream &operator<<(std::ostream &os, const Symbol &symbol) {
    return os << symbol.name();
}