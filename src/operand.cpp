#include "operand.h"

#include "utils.h"

const Register Register::RAX = Register(Register::Base::A, Register::Size::QWORD);
const Register Register::RBP = Register(Register::Base::BP, Register::Size::QWORD);
const Register Register::RSP = Register(Register::Base::SP, Register::Size::QWORD);
const Register Register::RDI = Register(Register::Base::DI, Register::Size::QWORD);
const Register Register::RSI = Register(Register::Base::SI, Register::Size::QWORD);
const Register Register::RDX = Register(Register::Base::D, Register::Size::QWORD);
const Register Register::RCX = Register(Register::Base::C, Register::Size::QWORD);
const Register Register::R8 = Register(Register::Base::R8, Register::Size::QWORD);
const Register Register::R9 = Register(Register::Base::R9, Register::Size::QWORD);
const Register Register::R11 = Register(Register::Base::R11, Register::Size::QWORD);

std::ostream &operator<<(std::ostream &os, const Memory &memory) {
    os << "[" << memory.base();

    if (memory.index() != 1) {
        os << " + " << memory.index();
    }

    if (memory.scale() != 1) {
        os << " * " << memory.scale();
    }

    if (memory.displacement() < 0) {
        os << " - " << std::abs(memory.displacement());
    } else if (memory.displacement() > 0) {
        os << " + " << std::abs(memory.displacement());
    }

    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Immediate &immediate) {
    std::visit([&os](const auto &arg) { os << arg; }, immediate.data());
    return os;
}

std::ostream &operator<<(std::ostream &os, const Register &reg) {
    if (reg.base() >= Register::Base::R8 && reg.base() <= Register::Base::R15) {
        switch (reg.size()) {
            case Register::Size::BYTE: return os << reg.base() << "b";
            case Register::Size::WORD: return os << reg.base() << "w";
            case Register::Size::DWORD: return os << reg.base() << "d";
            case Register::Size::QWORD: return os << reg.base();
            default: throw std::invalid_argument("This is not a valid register size.");
        }
    } else if (reg.base() >= Register::Base::SI && reg.base() <= Register::Base::BP) {
        switch (reg.size()) {
            case Register::Size::BYTE: return os << reg.base() << "l";
            case Register::Size::WORD: return os << reg.base();
            case Register::Size::DWORD: return os << "e" << reg.base();
            case Register::Size::QWORD: return os << "r" << reg.base();
            default: throw std::invalid_argument("This is not a valid register size.");
        }
    } else if (reg.base() >= Register::Base::A && reg.base() <= Register::Base::B) {
        switch (reg.size()) {
            case Register::Size::BYTE: return os << reg.base() << "l";
            case Register::Size::WORD: return os << reg.base() << "x";
            case Register::Size::DWORD: return os << "e" << reg.base() << "x";
            case Register::Size::QWORD: return os << "r" << reg.base() << "x";
            default: throw std::invalid_argument("This is not a valid register size.");
        }
    }

    throw std::invalid_argument("This register is not implemented.");
}

std::ostream &operator<<(std::ostream &os, const Register::Base &reg) {
    switch (reg) {
        case Register::Base::A: return os << "a";
        case Register::Base::C: return os << "c";
        case Register::Base::D: return os << "d";
        case Register::Base::B: return os << "b";
        case Register::Base::SI: return os << "si";
        case Register::Base::DI: return os << "di";
        case Register::Base::SP: return os << "sp";
        case Register::Base::BP: return os << "bp";
        case Register::Base::R8: return os << "r8";
        case Register::Base::R9: return os << "r9";
        case Register::Base::R10: return os << "r10";
        case Register::Base::R11: return os << "r11";
        case Register::Base::R12: return os << "r12";
        case Register::Base::R13: return os << "r13";
        case Register::Base::R14: return os << "r14";
        case Register::Base::R15: return os << "r15";
        default: throw std::invalid_argument("This register is not implemented.");
    }
}

Register::Size Register::type_to_register_size(const std::shared_ptr<Type> &type) {
    if (auto integer = std::dynamic_pointer_cast<IntegerType>(type)) {
        switch (integer->size()) {
            case 8: return Register::Size::BYTE;
            case 16: return Register::Size::WORD;
            case 32: return Register::Size::DWORD;
            case 64: return Register::Size::QWORD;
        }
    }

    throw std::runtime_error("This type is not implemented.");
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    if (auto symbol = std::get_if<std::shared_ptr<Symbol>>(&operand.data())) {
        return os << *symbol->get();
    }

    std::visit([&os](const auto &arg) { os << arg; }, operand.data());
    return os;
}