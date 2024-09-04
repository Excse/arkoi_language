#ifndef ARKOI_LANGUAGE_TYPE_H
#define ARKOI_LANGUAGE_TYPE_H

#include <optional>
#include <cstddef>
#include <cstdint>
#include <variant>
#include <memory>
#include <limits>

class Type {
public:
    virtual ~Type() = default;

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
    static const std::shared_ptr<IntegerType> TYPE_U8;
    static const std::shared_ptr<IntegerType> TYPE_S8;
    static const std::shared_ptr<IntegerType> TYPE_U16;
    static const std::shared_ptr<IntegerType> TYPE_S16;
    static const std::shared_ptr<IntegerType> TYPE_U32;
    static const std::shared_ptr<IntegerType> TYPE_S32;
    static const std::shared_ptr<IntegerType> TYPE_U64;
    static const std::shared_ptr<IntegerType> TYPE_S64;
    static const std::shared_ptr<IntegerType> TYPE_USize;
    static const std::shared_ptr<IntegerType> TYPE_SSize;

public:
    IntegerType(int64_t size, bool sign) : _size(size), _sign(sign) {}

    explicit IntegerType(uint64_t value) : _size(32), _sign(false) {
        while (_size < 64 && value > max()) _size *= 2;
    }

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

#endif //ARKOI_LANGUAGE_TYPE_H
