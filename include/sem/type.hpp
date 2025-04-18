#pragma once

#include <cstdint>

#include "utils/size.hpp"

namespace arkoi::sem {

class TypeBase {
public:
    virtual ~TypeBase() = default;

    [[nodiscard]] virtual Size size() const = 0;
};

class Integral final : public TypeBase {
public:
    constexpr Integral(const Size size, const bool sign) : _size(size), _sign(sign) {}

    [[nodiscard]] Size size() const override { return _size; }

    bool operator==(const Integral &other) const;

    bool operator!=(const Integral &other) const;

    [[nodiscard]] uint64_t max() const;

    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

class Floating final : public TypeBase {
public:
    explicit Floating(const Size size) : _size(size) {}

    [[nodiscard]] Size size() const override { return _size; }

    bool operator==(const Floating &other) const;

    bool operator!=(const Floating &other) const;

private:
    Size _size;
};

class Boolean final : public TypeBase {
public:
    [[nodiscard]] Size size() const override { return Size::BYTE; }

    bool operator==(const Boolean &other) const;

    bool operator!=(const Boolean &other) const;
};

struct Type final : TypeBase, std::variant<Integral, Floating, Boolean> {
    using variant::variant;

    [[nodiscard]] Size size() const override;
};

} // namespace arkoi::sem

std::ostream &operator<<(std::ostream &os, const arkoi::sem::Integral &type);

std::ostream &operator<<(std::ostream &os, const arkoi::sem::Floating &type);

std::ostream &operator<<(std::ostream &os, const arkoi::sem::Boolean &type);

std::ostream &operator<<(std::ostream &os, const arkoi::sem::Type &type);

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
