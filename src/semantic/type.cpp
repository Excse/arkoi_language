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

bool IntegralType::operator==(const IntegralType &other) const {
    return _size == other._size && _sign == other._sign;
}

bool IntegralType::operator!=(const IntegralType &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const IntegralType &type) {
    return os << (type.sign() ? "s" : "u") << size_to_bits(type.size());
}

uint64_t IntegralType::max() const {
    switch (_size) {
        case Size::BYTE: return _sign ? std::numeric_limits<int8_t>::max() : std::numeric_limits<uint8_t>::max();
        case Size::WORD: return _sign ? std::numeric_limits<int16_t>::max() : std::numeric_limits<uint16_t>::max();
        case Size::DWORD: return _sign ? std::numeric_limits<int32_t>::max() : std::numeric_limits<uint32_t>::max();
        case Size::QWORD: return _sign ? std::numeric_limits<int64_t>::max() : std::numeric_limits<uint64_t>::max();
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool FloatingType::operator==(const FloatingType &other) const {
    return _size == other._size;
}

bool FloatingType::operator!=(const FloatingType &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const FloatingType &type) {
    return os << "f" << size_to_bits(type.size());
}

std::ostream &operator<<(std::ostream &os, const BooleanType &) {
    return os << "bool";
}

bool BooleanType::operator==(const BooleanType &) const {
    return true;
}

bool BooleanType::operator!=(const BooleanType &) const {
    return false;
}
