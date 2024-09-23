#ifndef ARKOI_LANGUAGE_TYPE_H
#define ARKOI_LANGUAGE_TYPE_H

#include <cstdint>
#include <memory>

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]] virtual bool equals(const Type &other) const = 0;

    virtual std::ostream &print(std::ostream &os) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Type &type);

    friend bool operator==(const Type &lhs, const Type &rhs) { return lhs.equals(rhs); }

    friend bool operator!=(const Type &lhs, const Type &rhs) { return !(lhs == rhs); }
};

class IntegerType : public Type {
public:
    IntegerType(const int64_t size, const bool sign) : _size(size), _sign(sign) {}

    [[nodiscard]] bool equals(const Type &other) const override;

    std::ostream &print(std::ostream &os) const override;

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

class FloatingType : public Type {
public:
    explicit FloatingType(const int64_t size) : _size(size) {}

    [[nodiscard]] bool equals(const Type &other) const override;

    std::ostream &print(std::ostream &os) const override;

    friend std::ostream &operator<<(std::ostream &os, const FloatingType &type);

    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _size;
};

#endif //ARKOI_LANGUAGE_TYPE_H
