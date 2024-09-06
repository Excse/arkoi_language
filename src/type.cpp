#include "type.h"

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

std::ostream &operator<<(std::ostream &os, const Type &type) {
    if (auto integer = dynamic_cast<const IntegerType *>(&type)) {
        return os << (integer->sign() ? "s" : "u") << integer->size();
    }

    throw std::runtime_error("This type is not implemented.");
}

bool IntegerType::equals(const Type &other) const {
    const auto *to_check = dynamic_cast<const IntegerType *>(&other);
    if (!to_check) return false;

    return _size == to_check->_size && _sign == to_check->_sign;
}
