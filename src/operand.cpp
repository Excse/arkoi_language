#include "operand.h"

#include "utils.h"

std::ostream &Register::print(std::ostream &os) const {
    if (_base >= Base::R8 && _base <= Base::R15) {
        switch (_size) {
            case Size::BYTE: return os << _base << "b";
            case Size::WORD: return os << _base << "w";
            case Size::DWORD: return os << _base << "d";
            case Size::QWORD: return os << _base;
        }
    } else if (_base >= Base::SI && _base <= Base::BP) {
        switch (_size) {
            case Size::BYTE: return os << _base << "l";
            case Size::WORD: return os << _base;
            case Size::DWORD: return os << "e" << _base;
            case Size::QWORD: return os << "r" << _base;
        }
    } else if (_base >= Base::A && _base <= Base::B) {
        switch (_size) {
            case Size::BYTE: return os << _base << "l";
            case Size::WORD: return os << _base << "x";
            case Size::DWORD: return os << "e" << _base << "x";
            case Size::QWORD: return os << "r" << _base << "x";
        }
    } else if (_base >= Base::XMM0 && _base <= Base::XMM15) {
        return os << _base;
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
        case Register::Base::XMM0: return os << "xmm0";
        case Register::Base::XMM1: return os << "xmm1";
        case Register::Base::XMM2: return os << "xmm2";
        case Register::Base::XMM3: return os << "xmm3";
        case Register::Base::XMM4: return os << "xmm4";
        case Register::Base::XMM5: return os << "xmm5";
        case Register::Base::XMM6: return os << "xmm6";
        case Register::Base::XMM7: return os << "xmm7";
        case Register::Base::XMM8: return os << "xmm8";
        case Register::Base::XMM9: return os << "xmm9";
        case Register::Base::XMM10: return os << "xmm10";
        case Register::Base::XMM11: return os << "xmm11";
        case Register::Base::XMM12: return os << "xmm12";
        case Register::Base::XMM13: return os << "xmm13";
        case Register::Base::XMM14: return os << "xmm14";
        case Register::Base::XMM15: return os << "xmm15";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

Register::Size Register::type_to_register_size(const Type &type) {
    if (const auto integer = std::get_if<IntegerType>(&type)) {
        switch (integer->size()) {
            case 8: return Size::BYTE;
            case 16: return Size::WORD;
            case 32: return Size::DWORD;
            case 64: return Size::QWORD;
            default: throw std::invalid_argument("This is a invalid integer type size.");
        }
    } else if (const auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 32: return Size::DWORD;
            case 64: return Size::QWORD;
            default: throw std::invalid_argument("This is a invalid floating type size.");
        }
    }

    throw std::runtime_error("This type is not implemented.");
}

std::ostream &Memory::print(std::ostream &os) const {
    os << "[" << _base;

    if (_index != 1) {
        os << " + " << _index;
    }

    if (_scale != 1) {
        os << " * " << _scale;
    }

    if (_displacement < 0) {
        os << " - " << std::abs(_displacement);
    } else if (_displacement > 0) {
        os << " + " << std::abs(_displacement);
    }

    os << "]";
    return os;
}

std::ostream &Immediate::print(std::ostream &os) const {
    std::visit([&os](const auto &arg) { os << arg; }, _data);
    return os;
}

std::ostream &SymbolOperand::print(std::ostream &os) const {
    return os << *_symbol;
}
