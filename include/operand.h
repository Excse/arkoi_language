#pragma once

#include <iostream>
#include <utility>
#include <variant>
#include <cassert>
#include <memory>

#include "symbol_table.h"

class Register {
public:
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

    friend std::ostream &operator<<(std::ostream &os, const Register::Base &reg);

    enum class Size {
        BYTE, WORD, DWORD, QWORD
    };

public:
    Register(Base base, Size size) : _size(size), _base(base) {}

    friend std::ostream &operator<<(std::ostream &os, const Register &reg);

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto base() const { return _base; }

    static Size type_to_register_size(const Type &type);

private:
    Size _size;
    Base _base;
};

class Memory {
public:
    explicit Memory(Register base, int64_t index, int64_t scale, int64_t displacement)
            : _index(index), _scale(scale), _displacement(displacement), _base(std::move(base)) {}

    explicit Memory(Register base, int64_t displacement)
            : _index(1), _scale(1), _displacement(displacement), _base(std::move(base)) {}

    friend std::ostream &operator<<(std::ostream &os, const Memory &memory);

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto &base() const { return _base; }

private:
    int64_t _index, _scale, _displacement;
    Register _base;
};

struct Immediate : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float> {
    using variant::variant;

    friend std::ostream &operator<<(std::ostream &os, const Immediate &immediate);
};

class SymbolOperand {
public:
    explicit SymbolOperand(std::shared_ptr<Symbol> symbol) : _symbol(std::move(symbol)) {}

    friend std::ostream &operator<<(std::ostream &os, const SymbolOperand &symbol);

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
};

struct Operand : std::variant<SymbolOperand, Register, Memory, Immediate, std::monostate> {
    using variant::variant;

    friend std::ostream &operator<<(std::ostream &os, const Operand &operand);
};