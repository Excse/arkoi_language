#pragma once

#include <cstdint>

#include "size.hpp"

namespace arkoi::type {

class Integral {
public:
    constexpr Integral(const Size size, const bool sign) : _size(size), _sign(sign) {}

    bool operator==(const Integral &other) const;

    bool operator!=(const Integral &other) const;

    [[nodiscard]] uint64_t max() const;

    [[nodiscard]] auto size() const { return _size; }

    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

class Floating {
public:
    Floating(const Size size) : _size(size) {}

    bool operator==(const Floating &other) const;

    bool operator!=(const Floating &other) const;

    [[nodiscard]] auto size() const { return _size; }

private:
    Size _size;
};

class Boolean {
public:
    bool operator==(const Boolean &other) const;

    bool operator!=(const Boolean &other) const;

    [[nodiscard]] static auto size() { return Size::BYTE; }
};

} // namespace arkoi::type

struct Type : std::variant<arkoi::type::Integral, arkoi::type::Floating, arkoi::type::Boolean> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

std::ostream &operator<<(std::ostream &os, const arkoi::type::Integral &type);

std::ostream &operator<<(std::ostream &os, const arkoi::type::Floating &type);

std::ostream &operator<<(std::ostream &os, const arkoi::type::Boolean &type);

std::ostream &operator<<(std::ostream &os, const Type &type);

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
