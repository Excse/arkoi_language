#pragma once

#include <cstddef>
#include <utility>

#include "mid/operand.hpp"
#include "def/size.hpp"

namespace arkoi::back {

class OperandBase {
public:
    virtual ~OperandBase() = default;
};

class RegisterBase : public OperandBase {
public:
    using RegisterID = std::size_t;

public:
    RegisterBase(RegisterID id, Size size) : _reg_id(id), _size(size) {}

    bool operator==(const RegisterBase &other) const;

    bool operator!=(const RegisterBase &other) const;

    [[nodiscard]] auto reg_id() const { return _reg_id; }

    [[nodiscard]] auto size() const { return _size; }

private:
    RegisterID _reg_id;
    Size _size;
};

class MemoryBase : public OperandBase {
public:
    using LabelAddress = std::string;
    using DirectAddress = int64_t;

    struct Address : std::variant<LabelAddress, DirectAddress, RegisterBase> {
        using variant::variant;
    };

public:
    MemoryBase(Size size, Address address, int64_t index = 1, int64_t scale = 1, int64_t displacement = 1)
        : _index(index), _scale(scale), _displacement(displacement), _address(std::move(address)), _size(size) {}

    bool operator==(const MemoryBase &other) const;

    bool operator!=(const MemoryBase &other) const;

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto &address() const { return _address; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto &size() const { return _size; }

private:
    int64_t _index, _scale, _displacement;
    Address _address;
    Size _size;
};

class ConstantBase : public OperandBase, public mid::Constant {
public:
    using mid::Constant::Constant;

    ConstantBase(mid::Constant constant);
};

} // namespace arkoi::back