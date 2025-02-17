#pragma once

#include <iostream>
#include <variant>

#include "def/size.hpp"

namespace arkoi::mid {

class Variable {
public:
    Variable(std::string name, size_t version = 0)
        : _name(std::move(name)), _version(version) {}

    bool operator==(const Variable &rhs) const;

    bool operator!=(const Variable &rhs) const;

    [[nodiscard]] auto version() const { return _version; }

    [[nodiscard]] auto &symbol() const { return _name; }

private:
    std::string _name;
    size_t _version;
};

class Immediate : public std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
public:
    using variant::variant;

    [[nodiscard]] Size size() const;
};

struct Operand : std::variant<Immediate, Variable> {
public:
    using variant::variant;
};

} // namespace arkoi::mid

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Immediate &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Variable &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Operand &operand);

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
