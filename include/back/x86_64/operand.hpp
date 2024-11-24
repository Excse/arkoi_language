#pragma once

#include <iostream>
#include <utility>
#include <variant>

#include "back/mir/operand.hpp"

namespace arkoi::back::x86_64 {

class Register : public RegisterBase {
public:
    enum class Base : size_t {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

public:
    Register(Base base, Size size) : RegisterBase(std::to_underlying<Base>(base), size) {}

    Register(const RegisterBase &base) : RegisterBase(base.reg_id(), base.size()) {}

    [[nodiscard]] auto base() const { return static_cast<Base>(reg_id()); }

    [[nodiscard]] std::string base_name() const { return _base_name(base()); }

private:
    [[nodiscard]] static std::string _base_name(Base base);
};

class Memory : public MemoryBase {
public:
    Memory(Size size, Register reg, int64_t index, int64_t scale, int64_t displacement)
        : MemoryBase(size, reg, index, scale, displacement) {}

    Memory(Size size, Register reg, int64_t displacement)
        : MemoryBase(size, reg, 1, 1, displacement) {}

    Memory(Size size, Address address)
        : MemoryBase(size, std::move(address)) {}
};

struct Constant : public ConstantBase {
    using ConstantBase::ConstantBase;
};

struct Operand : std::variant<Register, Memory, Constant> {
    using variant::variant;
};

} // namespace arkoi::back::x86_64

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Register &reg);

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Memory &memory);

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Constant &constant);

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Operand &operand);