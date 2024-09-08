#ifndef ARKOI_LANGUAGE_OPERAND_H
#define ARKOI_LANGUAGE_OPERAND_H

#include <iostream>
#include <utility>
#include <variant>
#include <cassert>
#include <memory>

#include "symbol_table.h"

class Operand {
public:
    virtual ~Operand() = default;

    virtual std::ostream &print(std::ostream &os) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Operand &operand) { return operand.print(os); }
};

class Register : public Operand {
public:
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15
    };

    enum class Size {
        BYTE, WORD, DWORD, QWORD
    };

public:
    Register(Base base, Size size) : _size(size), _base(base) {}

    std::ostream &print(std::ostream &os) const override;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto base() const { return _base; }

    friend std::ostream &operator<<(std::ostream &os, const Register::Base &reg);

    static Size type_to_register_size(const std::shared_ptr<Type> &type);

private:
    Size _size;
    Base _base;
};

class Memory : public Operand {
public:
    explicit Memory(Register base, int64_t index, int64_t scale, int64_t displacement)
            : _index(index), _scale(scale), _displacement(displacement), _base(std::move(base)) {}

    explicit Memory(Register base, int64_t displacement)
            : _index(1), _scale(1), _displacement(displacement), _base(std::move(base)) {}

    std::ostream &print(std::ostream &os) const override;

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto &base() const { return _base; }

private:
    int64_t _index, _scale, _displacement;
    Register _base;
};

class Immediate : public Operand {
public:
    using Data = std::variant<uint64_t, int64_t, uint32_t, int32_t>;

    explicit Immediate(Data data) : _data(data) {}

    std::ostream &print(std::ostream &os) const override;

    [[nodiscard]] auto &data() const { return _data; }

private:
    Data _data;
};

class SymbolOperand : public Operand {
public:
    explicit SymbolOperand(std::shared_ptr<Symbol> symbol) : _symbol(std::move(symbol)) {}

    std::ostream &print(std::ostream &os) const override;

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
};

#endif //ARKOI_LANGUAGE_OPERAND_H
