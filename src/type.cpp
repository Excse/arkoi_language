#include "type.h"

#include <stdexcept>

const std::shared_ptr<IntegerType> IntegerType::TYPE_U8 = std::make_shared<IntegerType>(8, false);
const std::shared_ptr<IntegerType> IntegerType::TYPE_S8 = std::make_shared<IntegerType>(8, true);
const std::shared_ptr<IntegerType> IntegerType::TYPE_U16 = std::make_shared<IntegerType>(16, false);
const std::shared_ptr<IntegerType> IntegerType::TYPE_S16 = std::make_shared<IntegerType>(16, true);
const std::shared_ptr<IntegerType> IntegerType::TYPE_U32 = std::make_shared<IntegerType>(32, false);
const std::shared_ptr<IntegerType> IntegerType::TYPE_S32 = std::make_shared<IntegerType>(32, true);
const std::shared_ptr<IntegerType> IntegerType::TYPE_U64 = std::make_shared<IntegerType>(64, false);
const std::shared_ptr<IntegerType> IntegerType::TYPE_S64 = std::make_shared<IntegerType>(64, true);
const std::shared_ptr<IntegerType> IntegerType::TYPE_USize = std::make_shared<IntegerType>(64, false);
const std::shared_ptr<IntegerType> IntegerType::TYPE_SSize = std::make_shared<IntegerType>(64, true);

bool IntegerType::can_implicitly_cast_to(const Type &to) const {
    if (auto other = dynamic_cast<const IntegerType *>(&to)) {
        return _size <= other->_size && _sign == other->_sign;
    }

    throw std::invalid_argument("Implicit casting not implement for this case.");
}

bool IntegerType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const IntegerType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size && _sign == to_check->_sign;
}
