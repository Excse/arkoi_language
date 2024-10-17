#pragma once

#include <cstdint>
#include <memory>

#include "utils/data.hpp"

namespace arkoi::type {

class IntegralType {
public:
    IntegralType(const Size size, const bool sign) : _size(size), _sign(sign) {}

    bool operator==(const IntegralType &other) const;

    bool operator!=(const IntegralType &other) const;

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

    [[nodiscard]] auto size() const { return _size; }

private:
    Size _size;
};

class BooleanType {
public:
    bool operator==(const BooleanType &other) const;

    bool operator!=(const BooleanType &other) const;

    [[nodiscard]] static auto size() { return Size::BYTE; }
};

struct Type : std::variant<IntegralType, FloatingType, BooleanType> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

}

std::ostream &operator<<(std::ostream &os, const arkoi::type::IntegralType &type);

std::ostream &operator<<(std::ostream &os, const arkoi::type::FloatingType &type);

std::ostream &operator<<(std::ostream &os, const arkoi::type::BooleanType &type);

std::ostream &operator<<(std::ostream &os, const arkoi::type::Type &type);