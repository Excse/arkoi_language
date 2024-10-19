#include "intermediate/operand.hpp"

#include "utils/utils.hpp"

bool Register::operator==(const Register &other) const {
    return _size == other._size && _base == other._base;
}

bool Register::operator!=(const Register &other) const {
    return !(other == *this);
}

bool Memory::operator==(const Memory &other) const {
    return _index == other._index && _scale == other._scale && _displacement == other._displacement &&
           _address == other._address && _size == other._size;
}

bool Memory::operator!=(const Memory &other) const {
    return !(other == *this);
}

Size Immediate::size() const {
    return std::visit(match{
        [](const double &) { return Size::QWORD; },
        [](const float &) { return Size::DWORD; },
        [](const bool &) { return Size::BYTE; },
        [](const uint32_t &) { return Size::DWORD; },
        [](const int32_t &) { return Size::DWORD; },
        [](const uint64_t &) { return Size::QWORD; },
        [](const int64_t &) { return Size::QWORD; },
    }, *this);
}

std::ostream &operator<<(std::ostream &os, const Register &reg) {
    using Base = Register::Base;

    if (reg.base() >= Base::R8 && reg.base() <= Base::R15) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "b";
            case Size::WORD: return os << reg.base() << "w";
            case Size::DWORD: return os << reg.base() << "d";
            case Size::QWORD: return os << reg.base();
        }
    }

    if (reg.base() >= Base::SI && reg.base() <= Base::BP) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "l";
            case Size::WORD: return os << reg.base();
            case Size::DWORD: return os << "e" << reg.base();
            case Size::QWORD: return os << "r" << reg.base();
        }
    }

    if (reg.base() >= Base::A && reg.base() <= Base::B) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "l";
            case Size::WORD: return os << reg.base() << "x";
            case Size::DWORD: return os << "e" << reg.base() << "x";
            case Size::QWORD: return os << "r" << reg.base() << "x";
        }
    }

    if (reg.base() >= Base::XMM0 && reg.base() <= Base::XMM15) {
        return os << reg.base();
    }

    throw std::invalid_argument("This register is not implemented.");
}

std::ostream &operator<<(std::ostream &os, const Register::Base &reg) {
    switch (reg) {
        using Base = Register::Base;
        case Base::A: return os << "a";
        case Base::C: return os << "c";
        case Base::D: return os << "d";
        case Base::B: return os << "b";
        case Base::SI: return os << "si";
        case Base::DI: return os << "di";
        case Base::SP: return os << "sp";
        case Base::BP: return os << "bp";
        case Base::R8: return os << "r8";
        case Base::R9: return os << "r9";
        case Base::R10: return os << "r10";
        case Base::R11: return os << "r11";
        case Base::R12: return os << "r12";
        case Base::R13: return os << "r13";
        case Base::R14: return os << "r14";
        case Base::R15: return os << "r15";
        case Base::XMM0: return os << "xmm0";
        case Base::XMM1: return os << "xmm1";
        case Base::XMM2: return os << "xmm2";
        case Base::XMM3: return os << "xmm3";
        case Base::XMM4: return os << "xmm4";
        case Base::XMM5: return os << "xmm5";
        case Base::XMM6: return os << "xmm6";
        case Base::XMM7: return os << "xmm7";
        case Base::XMM8: return os << "xmm8";
        case Base::XMM9: return os << "xmm9";
        case Base::XMM10: return os << "xmm10";
        case Base::XMM11: return os << "xmm11";
        case Base::XMM12: return os << "xmm12";
        case Base::XMM13: return os << "xmm13";
        case Base::XMM14: return os << "xmm14";
        case Base::XMM15: return os << "xmm15";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::ostream &operator<<(std::ostream &os, const Memory::Address &memory) {
    std::visit(match{
        [](const std::monostate &) {},
        [&os](const auto &value) { os << value; },
    }, memory);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Memory &memory) {
    os << memory.size() << " PTR ";

    os << "[" << memory.address();

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
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, immediate);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit(match{
        [](const std::monostate &) {},
        [&os](const std::shared_ptr<Symbol> &symbol) { os << *symbol; },
        [&os](const auto &other) { os << other; },
    }, operand);
    return os;
}