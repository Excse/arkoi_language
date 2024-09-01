#include "operand.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const FPRelative &operand) {
    os << "[" << Register::RBP << (operand.sign() ? " + " : " - ") << operand.offset() << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Register &reg) {
    switch (reg) {
        case Register::RAX: return os << "rax";
        case Register::EAX: return os << "eax";
        case Register::AX: return os << "ax";
        case Register::AL: return os << "al";

        case Register::RCX: return os << "rcx";
        case Register::ECX: return os << "ecx";
        case Register::CX: return os << "cx";
        case Register::CL: return os << "cl";

        case Register::RDX: return os << "rdx";
        case Register::EDX: return os << "edx";
        case Register::DX: return os << "dx";
        case Register::DL: return os << "dl";

        case Register::RBX: return os << "rbx";
        case Register::EBX: return os << "ebx";
        case Register::BX: return os << "bx";
        case Register::BL: return os << "bl";

        case Register::RSI: return os << "rsi";
        case Register::ESI: return os << "esi";
        case Register::SI: return os << "si";
        case Register::SIL: return os << "sil";

        case Register::RDI: return os << "rdi";
        case Register::EDI: return os << "edi";
        case Register::DI: return os << "di";
        case Register::DIL: return os << "dil";

        case Register::RSP: return os << "rsp";
        case Register::ESP: return os << "esp";
        case Register::SP: return os << "sp";
        case Register::SPL: return os << "spl";

        case Register::RBP: return os << "rbp";
        case Register::EBP: return os << "ebp";
        case Register::BP: return os << "bp";
        case Register::BPL: return os << "bpl";

        case Register::R8: return os << "r8";
        case Register::R8D: return os << "r8d";
        case Register::R8W: return os << "r8w";
        case Register::R8B: return os << "r8b";

        case Register::R9: return os << "r9";
        case Register::R9D: return os << "r9d";
        case Register::R9W: return os << "r9w";
        case Register::R9B: return os << "r9b";

        case Register::R10: return os << "r10";
        case Register::R10D: return os << "r10d";
        case Register::R10W: return os << "r10w";
        case Register::R10B: return os << "r10b";

        case Register::R11: return os << "r11";
        case Register::R11D: return os << "r11d";
        case Register::R11W: return os << "r11w";
        case Register::R11B: return os << "r11b";

        case Register::R12: return os << "r12";
        case Register::R12D: return os << "r12d";
        case Register::R12W: return os << "r12w";
        case Register::R12B: return os << "r12b";

        case Register::R13: return os << "r13";
        case Register::R13D: return os << "r13d";
        case Register::R13W: return os << "r13w";
        case Register::R13B: return os << "r13b";

        case Register::R14: return os << "r14";
        case Register::R14D: return os << "r14d";
        case Register::R14W: return os << "r14w";
        case Register::R14B: return os << "r14b";

        case Register::R15: return os << "r15";
        case Register::R15D: return os << "r15d";
        case Register::R15W: return os << "r15w";
        case Register::R15B: return os << "r15b";

        default: throw std::invalid_argument("This register is not printable.");
    }
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