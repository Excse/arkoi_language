#include "utils/data.hpp"

#include <utility>

std::ostream &operator<<(std::ostream &os, const Size &size) {
    switch (size) {
        case Size::BYTE: return os << "BYTE";
        case Size::WORD: return os << "WORD";
        case Size::DWORD: return os << "DWORD";
        case Size::QWORD: return os << "QWORD";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

size_t size_to_bytes(const Size &size) {
    return std::to_underlying(size);
}

size_t size_to_bits(const Size &size) {
    return 8 * size_to_bytes(size);
}