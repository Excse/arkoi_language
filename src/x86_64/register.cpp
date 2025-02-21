#include "x86_64/register.hpp"

#include <utility>

using namespace arkoi::x86_64;

bool Register::operator==(const Register &other) const {
    return _size == other._size && _base == other._base;
}

bool Register::operator!=(const Register &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Register &reg) {
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

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
