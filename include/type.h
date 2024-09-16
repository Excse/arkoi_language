#ifndef ARKOI_LANGUAGE_TYPE_H
#define ARKOI_LANGUAGE_TYPE_H

#include <optional>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <limits>

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]] virtual bool equals(const Type &other) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Type &reg);

    friend bool operator==(const Type &lhs, const Type &rhs) { return lhs.equals(rhs); }

    friend bool operator!=(const Type &lhs, const Type &rhs) { return !(lhs == rhs); }
};

class IntegerType : public Type {
public:

public:
    IntegerType(int64_t size, bool sign) : _size(size), _sign(sign) {}

    [[nodiscard]] bool equals(const Type &other) const override;

    [[nodiscard]] uint64_t max() const {
        if (_sign) return (1ULL << (_size - 1)) - 1;
        else return (1ULL << _size) - 1;
    }

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    int64_t _size;
    bool _sign;
};

class FloatingType : public Type {
public:

public:
    explicit FloatingType(int64_t size) : _size(size) {}

    [[nodiscard]] bool equals(const Type &other) const override;

    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _size;
};

#endif //ARKOI_LANGUAGE_TYPE_H
