#include "back/mir/operand.hpp"

#include "utils/utils.hpp"

using namespace arkoi::back;

bool RegisterBase::operator==(const RegisterBase &other) const {
    return _size == other._size && _reg_id == other._reg_id;
}

bool RegisterBase::operator!=(const RegisterBase &other) const {
    return !(other == *this);
}

bool MemoryBase::operator==(const MemoryBase &other) const {
    return _index == other._index && _scale == other._scale && _displacement == other._displacement &&
           _address == other._address && _size == other._size;
}

bool MemoryBase::operator!=(const MemoryBase &other) const {
    return !(other == *this);
}

ConstantBase::ConstantBase(mid::Constant constant) {
    std::visit([this](auto &&value) { this->operator=(value); }, constant);
}
