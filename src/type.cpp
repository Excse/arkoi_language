#include "type.h"

#include <stdexcept>

bool IntegerType::can_implicitly_cast_to(const Type &to) const {
    if (auto other = dynamic_cast<const IntegerType *>(&to)) {
        return _size <= other->_size && _sign == other->_sign;
    }

    throw std::runtime_error("Implicit casting not implement for this case.");
}

bool IntegerType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const IntegerType *>(&other);
    if(!to_check) return false;

    return _size == to_check->_size && _sign == to_check->_sign;
}
