#pragma once

#include <cstdint>
#include <memory>

class IntegerType {
public:
    IntegerType(const int64_t size, const bool sign) : _size(size), _sign(sign) {}

    bool operator==(const IntegerType &other) const;

    bool operator!=(const IntegerType &other) const;

    friend std::ostream &operator<<(std::ostream &os, const IntegerType &type);

    [[nodiscard]] uint64_t max() const {
        if (_sign) return (1ULL << (_size - 1)) - 1;
        return (1ULL << _size) - 1;
    }

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    int64_t _size;
    bool _sign;
};

class FloatingType {
public:
    explicit FloatingType(const int64_t size) : _size(size) {}

    bool operator==(const FloatingType &other) const;

    bool operator!=(const FloatingType &other) const;

    friend std::ostream &operator<<(std::ostream &os, const FloatingType &type);

    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _size;
};

struct Type : std::variant<std::monostate, IntegerType, FloatingType> {
    using variant::variant;

    friend std::ostream &operator<<(std::ostream &os, const Type &type);
};