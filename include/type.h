#ifndef ARKOI_LANGUAGE_TYPE_H
#define ARKOI_LANGUAGE_TYPE_H

#include <cstddef>
#include <cstdint>
#include <variant>
#include <limits>

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]] virtual bool can_implicitly_cast_to(const Type &other) const = 0;

    [[nodiscard]] virtual bool equals(const Type &other) const = 0;

    friend bool operator==(const Type &lhs, const Type &rhs) {
        return lhs.equals(rhs);
    }

    friend bool operator!=(const Type &lhs, const Type &rhs) {
        return !(lhs == rhs);
    }
};

class IntegerType : public Type {
public:
    explicit IntegerType(size_t size, bool sign) : _size(size), _sign(sign) {}

    explicit IntegerType(unsigned long long value) : _size(8), _sign(false) {
        while (_size < 64 && value > _max(_size)) _size *= 2;
    }

    [[nodiscard]] bool can_implicitly_cast_to(const Type &to) const override;

    [[nodiscard]] bool equals(const Type &other) const override;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    [[nodiscard]]
    static uint64_t _max(size_t size) {
        return (1ULL << size) - 1;
    }

private:
    size_t _size;
    bool _sign;
};

#endif //ARKOI_LANGUAGE_TYPE_H
