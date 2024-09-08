#include "operand.h"

#include "utils.h"

std::ostream &Register::print(std::ostream &os) const {
    if (_base >= Register::Base::R8 && _base <= Register::Base::R15) {
        switch (_size) {
            case Register::Size::BYTE: return os << _base << "b";
            case Register::Size::WORD: return os << _base << "w";
            case Register::Size::DWORD: return os << _base << "d";
            case Register::Size::QWORD: return os << _base;
            default: throw std::invalid_argument("This is not a valid register size.");
        }
    } else if (_base >= Register::Base::SI && _base <= Register::Base::BP) {
        switch (_size) {
            case Register::Size::BYTE: return os << _base << "l";
            case Register::Size::WORD: return os << _base;
            case Register::Size::DWORD: return os << "e" << _base;
            case Register::Size::QWORD: return os << "r" << _base;
            default: throw std::invalid_argument("This is not a valid register size.");
        }
    } else if (_base >= Register::Base::A && _base <= Register::Base::B) {
        switch (_size) {
            case Register::Size::BYTE: return os << _base << "l";
            case Register::Size::WORD: return os << _base << "x";
            case Register::Size::DWORD: return os << "e" << _base << "x";
            case Register::Size::QWORD: return os << "r" << _base << "x";
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
