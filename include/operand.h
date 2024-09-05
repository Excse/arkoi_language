#ifndef ARKOI_LANGUAGE_OPERAND_H
#define ARKOI_LANGUAGE_OPERAND_H

#include <iostream>
#include <variant>
#include <memory>

#include "symbol_table.h"

enum class Register {
    RAX, EAX, AX, AL,
    RCX, ECX, CX, CL,
    RDX, EDX, DX, DL,
    RBX, EBX, BX, BL,
    RSI, ESI, SI, SIL,
    RDI, EDI, DI, DIL,
    RSP, ESP, SP, SPL,
    RBP, EBP, BP, BPL,
    R8, R8D, R8W, R8B,
    R9, R9D, R9W, R9B,
    R10, R10D, R10W, R10B,
    R11, R11D, R11W, R11B,
    R12, R12D, R12W, R12B,
    R13, R13D, R13W, R13B,
    R14, R14D, R14W, R14B,
    R15, R15D, R15W, R15B,
};

class FPRelative {
public:
    explicit FPRelative(int64_t offset, bool sign) : _offset(offset), _sign(sign) {}

    [[nodiscard]] auto offset() const { return _offset; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    int64_t _offset;
    bool _sign;
};

using Operand = std::variant<std::shared_ptr<Symbol>, FPRelative, uint64_t, int64_t, uint32_t, int32_t, Register>;

std::ostream &operator<<(std::ostream &os, const FPRelative &operand);

std::ostream &operator<<(std::ostream &os, const Register &reg);

std::ostream &operator<<(std::ostream &os, const Operand &operand);

#endif //ARKOI_LANGUAGE_OPERAND_H
