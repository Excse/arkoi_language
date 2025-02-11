#include "back/x86_64/operand.hpp"

#include "utils/utils.hpp"

using namespace arkoi::back::x86_64;

std::string Register::_base_name(Register::Base base) {
    switch (base) {
        case Register::Base::A: return "a";
        case Register::Base::C: return "c";
        case Register::Base::D: return "d";
        case Register::Base::B: return "b";
        case Register::Base::SI: return "si";
        case Register::Base::DI: return "di";
        case Register::Base::SP: return "sp";
        case Register::Base::BP: return "bp";
        case Register::Base::R8: return "r8";
        case Register::Base::R9: return "r9";
        case Register::Base::R10: return "r10";
        case Register::Base::R11: return "r11";
        case Register::Base::R12: return "r12";
        case Register::Base::R13: return "r13";
        case Register::Base::R14: return "r14";
        case Register::Base::R15: return "r15";
        case Register::Base::XMM0: return "xmm0";
        case Register::Base::XMM1: return "xmm1";
        case Register::Base::XMM2: return "xmm2";
        case Register::Base::XMM3: return "xmm3";
        case Register::Base::XMM4: return "xmm4";
        case Register::Base::XMM5: return "xmm5";
        case Register::Base::XMM6: return "xmm6";
        case Register::Base::XMM7: return "xmm7";
        case Register::Base::XMM8: return "xmm8";
        case Register::Base::XMM9: return "xmm9";
        case Register::Base::XMM10: return "xmm10";
        case Register::Base::XMM11: return "xmm11";
        case Register::Base::XMM12: return "xmm12";
        case Register::Base::XMM13: return "xmm13";
        case Register::Base::XMM14: return "xmm14";
        case Register::Base::XMM15: return "xmm15";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Register &reg) {
    if (reg.base() >= Register::Base::R8 && reg.base() <= Register::Base::R15) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base_name() << "b";
            case Size::WORD: return os << reg.base_name() << "w";
            case Size::DWORD: return os << reg.base_name() << "d";
            case Size::QWORD: return os << reg.base_name();
        }
    }

    if (reg.base() >= Register::Base::SI && reg.base() <= Register::Base::BP) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base_name() + "l";
            case Size::WORD: return os << reg.base_name();
            case Size::DWORD: return os << "e" + reg.base_name();
            case Size::QWORD: return os << "r" + reg.base_name();
        }
    }

    if (reg.base() >= Register::Base::A && reg.base() <= Register::Base::B) {
        switch (reg.size()) {
            case Size::BYTE: return os << reg.base_name() + "l";
            case Size::WORD: return os << reg.base_name() + "x";
            case Size::DWORD: return os << "e" + reg.base_name() + "x";
            case Size::QWORD: return os << "r" + reg.base_name() + "x";
        }
    }

    if (reg.base() >= Register::Base::XMM0 && reg.base() <= Register::Base::XMM15) {
        return os << reg.base_name();
    }

    throw std::invalid_argument("This register is not implemented.");
}

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Memory &memory) {
    os << memory.size() << " PTR [";

    std::visit(match{
        [&](const arkoi::back::RegisterBase &base) {
            os << Register(base);
        },
        [&](const auto &value) {
            os << value;
        }
    }, memory.address());

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

std::ostream &operator<<(std::ostream &os, const arkoi::back::x86_64::Constant &constant) {
    std::visit([&os](const auto &value) { os << value; }, constant);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}
