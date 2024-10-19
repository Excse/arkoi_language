#pragma once

#include <iostream>
#include <utility>
#include <variant>
#include <cassert>
#include <memory>

#include "semantic/symbol_table.hpp"
#include "utils/data.hpp"

class Register {
public:
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

public:
    Register(Base base, Size size) : _size(size), _base(base) {}

    bool operator==(const Register &other) const;

    bool operator!=(const Register &other) const;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto base() const { return _base; }

private:
    Size _size;
    Base _base;
};

class Memory {
public:
    struct Address : std::variant<std::string, int64_t, Register> { };

public:
    explicit Memory(Size size, Register address, int64_t index, int64_t scale, int64_t displacement)
        : _index(index), _scale(scale), _displacement(displacement), _address(address), _size(size) {}

    explicit Memory(Size size, Register address, int64_t displacement)
        : _index(1), _scale(1), _displacement(displacement), _address(address), _size(size) {}

    explicit Memory(Size size, Address address)
        : _index(1), _scale(1), _displacement(0), _address(std::move(address)), _size(size) {}

    bool operator==(const Memory &other) const;

    bool operator!=(const Memory &other) const;

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

struct Immediate : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

struct Operand : std::variant<Register, Memory, std::shared_ptr<Symbol>, Immediate> {
    using variant::variant;
};

std::ostream &operator<<(std::ostream &os, const Register &reg);

std::ostream &operator<<(std::ostream &os, const Register::Base &reg);

std::ostream &operator<<(std::ostream &os, const Operand &operand);

std::ostream &operator<<(std::ostream &os, const Memory &memory);

std::ostream &operator<<(std::ostream &os, const Memory::Address &memory);

std::ostream &operator<<(std::ostream &os, const Immediate &immediate);
