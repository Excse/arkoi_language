#pragma once

#include <cstdint>
#include <memory>

#include "data.hpp"

class IntegralType {
public:
    IntegralType(const Size size, const bool sign) : _size(size), _sign(sign) {}

    bool operator==(const IntegralType &other) const;

    bool operator!=(const IntegralType &other) const;

    friend std::ostream &operator<<(std::ostream &os, const IntegralType &type);

    [[nodiscard]] uint64_t max() const;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

class FloatingType {
public:
    explicit FloatingType(const Size size) : _size(size) {}

    bool operator==(const FloatingType &other) const;

    bool operator!=(const FloatingType &other) const;

    friend std::ostream &operator<<(std::ostream &os, const FloatingType &type);

    [[nodiscard]] auto size() const { return _size; }

private:
    Size _size;
};

class BooleanType {
public:
    friend std::ostream &operator<<(std::ostream &os, const BooleanType &type);

    bool operator==(const BooleanType &other) const;

    bool operator!=(const BooleanType &other) const;

    [[nodiscard]] static auto size() { return Size::BYTE; }
};

struct Type : std::variant<IntegralType, FloatingType, BooleanType> {
    using variant::variant;

    friend std::ostream &operator<<(std::ostream &os, const Type &type);

    [[nodiscard]] Size size() const;
};