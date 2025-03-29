#pragma once

#include "utils/size.hpp"

namespace arkoi::x86_64 {

class Register {
public:
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

public:
    constexpr Register(Base base, Size size) : _size(size), _base(base) {}

    bool operator==(const Register &other) const;

    bool operator!=(const Register &other) const;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto base() const { return _base; }

private:
    Size _size;
    Base _base;
};

class Memory {
public:
    struct Address : public std::variant<std::string, int64_t, Register> {
        using variant::variant;
    };

public:
    Memory(Size size, Register address, int64_t index, int64_t scale, int64_t displacement)
        : _index(index), _scale(scale), _displacement(displacement), _address(address), _size(size) {}

    Memory(Size size, Register address, int64_t displacement)
        : _index(1), _scale(1), _displacement(displacement), _address(address), _size(size) {}

    Memory(Size size, Address address)
        : _index(1), _scale(1), _displacement(0), _address(std::move(address)), _size(size) {}

    bool operator==(const Memory &other) const;

    bool operator!=(const Memory &other) const;

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto &address() const { return _address; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _index, _scale, _displacement;
    Address _address;
    Size _size;
};

struct Immediate : public std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;
};

struct Operand : public std::variant<Memory, Register, Immediate> {
    using variant::variant;
};

} // namespace arkoi::x86_64

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Register &reg);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Register::Base &reg);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Memory &memory);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Memory::Address &memory);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Immediate &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::x86_64::Operand &mapping);

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
