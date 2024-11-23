#include "back/x86_64/operand.hpp"

#include "utils/utils.hpp"

using namespace arkoi::back::x86_64;

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

std::ostream &operator<<(std::ostream &os, const Register &reg) {
    if (reg.base() >= Register::Base::R8 && reg.base() <= Register::Base::R15) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "b";
            case Size::WORD: return os << reg.base() << "w";
            case Size::DWORD: return os << reg.base() << "d";
            case Size::QWORD: return os << reg.base();
        }
    }

    if (reg.base() >= Register::Base::SI && reg.base() <= Register::Base::BP) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "l";
            case Size::WORD: return os << reg.base();
            case Size::DWORD: return os << "e" << reg.base();
            case Size::QWORD: return os << "r" << reg.base();
        }
    }

    if (reg.base() >= Register::Base::A && reg.base() <= Register::Base::B) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base() << "l";
            case Size::WORD: return os << reg.base() << "x";
            case Size::DWORD: return os << "e" << reg.base() << "x";
            case Size::QWORD: return os << "r" << reg.base() << "x";
        }
    }

    if (reg.base() >= Register::Base::XMM0 && reg.base() <= Register::Base::XMM15) {
        return os << reg.base();
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

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}