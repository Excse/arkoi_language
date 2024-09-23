#include "type.h"

std::ostream &operator<<(std::ostream &os, const Type &type) {
    return type.print(os);
}

bool IntegerType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const IntegerType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size && _sign == to_check->_sign;
}

std::ostream &IntegerType::print(std::ostream &os) const {
    return os << (_sign ? "s" : "u") << _size;
}

std::ostream &operator<<(std::ostream &os, const IntegerType &type) {
    return type.print(os);
}

bool FloatingType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const FloatingType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size;
}

std::ostream &FloatingType::print(std::ostream &os) const {
    return os << "f" << _size;
}

std::ostream &operator<<(std::ostream &os, const FloatingType &type) {
    return type.print(os);
}
