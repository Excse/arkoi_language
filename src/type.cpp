#include "type.h"

std::ostream &operator<<(std::ostream &os, const Type &type) {
    if (auto integer = dynamic_cast<const IntegerType *>(&type)) {
        return os << (integer->sign() ? "s" : "u") << integer->size();
    } else if (auto floating = dynamic_cast<const FloatingType *>(&type)) {
        return os << "f" << floating->size();
    }

    throw std::runtime_error("This op is not implemented.");
}

bool IntegerType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const IntegerType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size && _sign == to_check->_sign;
}

bool FloatingType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const FloatingType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size;
}
