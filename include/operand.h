#ifndef ARKOI_LANGUAGE_OPERAND_H
#define ARKOI_LANGUAGE_OPERAND_H

#include <iostream>
#include <variant>
#include <memory>

#include "symbol_table.h"

class FPRelative {
public:
    explicit FPRelative(size_t offset, bool sign) : _offset(offset), _sign(sign) {}

    [[nodiscard]] auto offset() const { return _offset; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    size_t _offset;
    bool _sign;
};

using Operand = std::variant<std::shared_ptr<Symbol>, FPRelative, long long>;

std::ostream &operator<<(std::ostream &os, const FPRelative &operand);

std::ostream &operator<<(std::ostream &os, const Operand &operand);

#endif //ARKOI_LANGUAGE_OPERAND_H
