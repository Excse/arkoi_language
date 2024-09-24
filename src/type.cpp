#include "type.h"

std::ostream &operator<<(std::ostream &os, const Type &type) {
    std::visit([&os](const auto &value) { os << value; }, type);
    return os;
}

bool IntegerType::operator==(const IntegerType &other) const {
    return _size == other._size && _sign == other._sign;
}

std::ostream &operator<<(std::ostream &os, const IntegerType &type) {
    return os << (type.sign() ? "s" : "u") << type.size();
}

bool FloatingType::operator==(const FloatingType &other) const {
    return _size == other._size;
}

std::ostream &operator<<(std::ostream &os, const FloatingType &type) {
    return os << "f" << type.size();
}
