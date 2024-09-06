#ifndef ARKOI_LANGUAGE_OPERAND_H
#define ARKOI_LANGUAGE_OPERAND_H

#include <iostream>
#include <utility>
#include <variant>
#include <cassert>
#include <memory>

#include "symbol_table.h"

class Register {
public:
    static const Register RAX;
    static const Register RBP;
    static const Register RDI;
    static const Register RSI;
    static const Register RDX;
    static const Register RCX;
    static const Register R8;
    static const Register R9;
    static const Register R11;

    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15
    };

    enum class Size {
        BYTE, WORD, DWORD, QWORD
    };

public:
    Register(Base base, Size size) : _size(size), _base(base) {}

    [[nodiscard]] auto &size() const { return _size; }

    [[nodiscard]] auto &base() const { return _base; }

    friend std::ostream &operator<<(std::ostream &os, const Register::Base &reg);

    friend std::ostream &operator<<(std::ostream &os, const Register &reg);

private:
    Size _size;
    Base _base;
};

class Memory {
public:
    explicit Memory(Register base, int64_t index, int64_t scale, int64_t displacement)
            : _index(index), _scale(scale), _displacement(displacement), _base(base) {}

    explicit Memory(Register base, int64_t displacement)
            : _index(1), _scale(1), _displacement(displacement), _base(base) {}

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto &base() const { return _base; }

    friend std::ostream &operator<<(std::ostream &os, const Memory &memory);

private:
    int64_t _index, _scale, _displacement;
    Register _base;
};

class Immediate {
public:
    using Data = std::variant<uint64_t, int64_t, uint32_t, int32_t>;

    explicit Immediate(Data data) : _data(data) {}

    [[nodiscard]] auto &data() const { return _data; }

    friend std::ostream &operator<<(std::ostream &os, const Immediate &immediate);

private:
    Data _data;
};

class Operand {
public:
    using Data = std::variant<std::shared_ptr<Symbol>, Memory, Immediate, Register>;

public:
    explicit Operand(Data data) : _data(std::move(data)) {}

    explicit Operand() : _data() {}

    [[nodiscard]] auto &data() const { return _data; }

    friend std::ostream &operator<<(std::ostream &os, const Operand &operand);

private:
    Data _data;
};

#endif //ARKOI_LANGUAGE_OPERAND_H
