#include "semantic/type.hpp"

#include <utility>

using namespace arkoi::type;
using namespace arkoi;

std::ostream &operator<<(std::ostream &os, const Type &type) {
    std::visit([&os](const auto &value) { os << value; }, type);
    return os;
}

Size Type::size() const {
    return std::visit([](const auto &value) { return value.size(); }, *this);
}

bool Integral::operator==(const Integral &other) const {
    return _size == other._size && _sign == other._sign;
}

bool Integral::operator!=(const Integral &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const Integral &type) {
    return os << (type.sign() ? "s" : "u") << size_to_bits(type.size());
}

uint64_t Integral::max() const {
    switch (_size) {
        case Size::BYTE: return _sign ? std::numeric_limits<int8_t>::max() : std::numeric_limits<uint8_t>::max();
        case Size::WORD: return _sign ? std::numeric_limits<int16_t>::max() : std::numeric_limits<uint16_t>::max();
        case Size::DWORD: return _sign ? std::numeric_limits<int32_t>::max() : std::numeric_limits<uint32_t>::max();
        case Size::QWORD: return _sign ? std::numeric_limits<int64_t>::max() : std::numeric_limits<uint64_t>::max();
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool Floating::operator==(const Floating &other) const {
    return _size == other._size;
}

bool Floating::operator!=(const Floating &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const Floating &type) {
    return os << "f" << size_to_bits(type.size());
}

std::ostream &operator<<(std::ostream &os, const Boolean &) {
    return os << "bool";
}

bool Boolean::operator==(const Boolean &) const {
    return true;
}

bool Boolean::operator!=(const Boolean &) const {
    return false;
}
