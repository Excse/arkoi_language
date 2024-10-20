#pragma once

#include <cstdint>

#include "utils/data.hpp"

namespace type {

class Integral {
public:
    Integral(const Size size, const bool sign) : _size(size), _sign(sign) {}

    bool operator==(const Integral &other) const;

    bool operator!=(const Integral &other) const;

    [[nodiscard]] uint64_t max() const;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

class Floating {
public:
    explicit Floating(const Size size) : _size(size) {}

    bool operator==(const Floating &other) const;

    bool operator!=(const Floating &other) const;

    [[nodiscard]] auto size() const { return _size; }

private:
    Size _size;
};

class Boolean {
public:
    bool operator==(const Boolean &other) const;

    bool operator!=(const Boolean &other) const;

    [[nodiscard]] static auto size() { return Size::BYTE; }
};

}

struct Type : std::variant<type::Integral, type::Floating, type::Boolean> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

std::ostream &operator<<(std::ostream &os, const type::Integral &type);

std::ostream &operator<<(std::ostream &os, const type::Floating &type);

std::ostream &operator<<(std::ostream &os, const type::Boolean &type);

std::ostream &operator<<(std::ostream &os, const Type &type);