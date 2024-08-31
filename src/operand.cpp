#include "operand.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const FPRelative &operand) {
    if (operand.sign()) {
        os << "[rbp + " << to_string(operand.offset()) << "]";
    } else {
        os << "[rbp - " << to_string(operand.offset()) << "]";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Register &reg) {
    static const std::unordered_map<Register, std::string_view> REGISTERS = {
            {Register::RAX, "rax"}, {Register::EAX, "eax"}, {Register::AX, "ax"}, {Register::AL, "al"},
            {Register::RCX, "rcx"}, {Register::ECX, "ecx"}, {Register::CX, "cx"}, {Register::CL, "cl"},
            {Register::RDX, "rdx"}, {Register::EDX, "edx"}, {Register::DX, "dx"}, {Register::DL, "dl"},
            {Register::RBX, "rbx"}, {Register::EBX, "ebx"}, {Register::BX, "bx"}, {Register::BL, "bl"},
            {Register::RSI, "rsi"}, {Register::ESI, "esi"}, {Register::SI, "si"}, {Register::SIL, "sil"},
            {Register::RDI, "rdi"}, {Register::EDI, "edi"}, {Register::DI, "di"}, {Register::DIL, "dil"},
            {Register::RSP, "rsp"}, {Register::ESP, "esp"}, {Register::SP, "sp"}, {Register::SPL, "spl"},
            {Register::RBP, "rbp"}, {Register::EBP, "ebp"}, {Register::BP, "bp"}, {Register::BPL, "bpl"},
            {Register::R8,  "r8"},  {Register::R8D, "r8d"}, {Register::R8W, "r8w"}, {Register::R8B, "r8b"},
            {Register::R9,  "r9"},  {Register::R9D, "r9d"}, {Register::R9W, "r9w"}, {Register::R9B, "r9b"},
            {Register::R10, "r10"}, {Register::R10D, "r10d"}, {Register::R10W, "r10w"}, {Register::R10B, "r10b"},
            {Register::R11, "r11"}, {Register::R11D, "r11d"}, {Register::R11W, "r11w"}, {Register::R11B, "r11b"},
            {Register::R12, "r12"}, {Register::R12D, "r12d"}, {Register::R12W, "r12w"}, {Register::R12B, "r12b"},
            {Register::R13, "r13"}, {Register::R13D, "r13d"}, {Register::R13W, "r13w"}, {Register::R13B, "r13b"},
            {Register::R14, "r14"}, {Register::R14D, "r14d"}, {Register::R14W, "r14w"}, {Register::R14B, "r14b"},
            {Register::R15, "r15"}, {Register::R15D, "r15d"}, {Register::R15W, "r15w"}, {Register::R15B, "r15b"},
    };

    const auto found = REGISTERS.find(reg);
    if (found != REGISTERS.end()) {
        return os << found->second;
    }

    throw std::invalid_argument("This register is not printable.");
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { os << *symbol; },
            [&](const FPRelative &relative) { os << relative; },
            [&](const long long &value) { os << value; },
            [&](const Register &reg) { os << reg; },
    }, operand);
    return os;
}