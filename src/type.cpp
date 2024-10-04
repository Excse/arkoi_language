#include "type.hpp"

std::ostream &operator<<(std::ostream &os, const Type &type) {
    std::visit([&os](const auto &value) { os << value; }, type);
    return os;
}

bool IntegralType::operator==(const IntegralType &other) const {
    return _size == other._size && _sign == other._sign;
}

bool IntegralType::operator!=(const IntegralType &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const IntegralType &type) {
    return os << (type.sign() ? "s" : "u") << type.size();
}

bool FloatingType::operator==(const FloatingType &other) const {
    return _size == other._size;
}

bool FloatingType::operator!=(const FloatingType &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const FloatingType &type) {
    return os << "f" << type.size();
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
