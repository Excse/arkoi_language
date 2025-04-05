#pragma once

#include <iostream>
#include <variant>

#include "utils/size.hpp"
#include "sem/type.hpp"

namespace arkoi::il {

class OperandBase {
public:
    virtual ~OperandBase() = default;

    [[nodiscard]] virtual sem::Type type() const = 0;
};

class Memory : public OperandBase {
public:
    Memory(size_t version, sem::Type type)
        : _type(std::move(type)), _index(version) {}

    bool operator<(const Memory& rhs) const;

    bool operator==(const Memory &rhs) const;

    bool operator!=(const Memory &rhs) const;

    [[nodiscard]] sem::Type type() const override { return _type; }

    [[nodiscard]] auto index() const { return _index; }

private:
    sem::Type _type;
    size_t _index;
};

class Variable : public OperandBase {
public:
    Variable(std::string name, sem::Type type, size_t version = 0)
        : _name(std::move(name)), _version(version), _type(std::move(type)) {}

    bool operator<(const Variable& rhs) const;

    bool operator==(const Variable &rhs) const;

    bool operator!=(const Variable &rhs) const;

    [[nodiscard]] sem::Type type() const override { return _type; }

    [[nodiscard]] auto version() const { return _version; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::string _name;
    size_t _version;
    sem::Type _type;
};

struct Immediate : public OperandBase, public std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    [[nodiscard]] sem::Type type() const override;
};

struct Operand : public OperandBase, public std::variant<Immediate, Variable, Memory> {
    using variant::variant;

    [[nodiscard]] sem::Type type() const override;
};

} // namespace arkoi::il

namespace std {

template<>
struct hash<arkoi::il::Variable> {
    size_t operator()(const arkoi::il::Variable &variable) const;
};

template<>
struct hash<arkoi::il::Memory> {
    size_t operator()(const arkoi::il::Memory &memory) const;
};

template<>
struct hash<arkoi::il::Immediate> {
    size_t operator()(const arkoi::il::Immediate &immediate) const;
};

template<>
struct hash<arkoi::il::Operand> {
    size_t operator()(const arkoi::il::Operand &operand) const;
};

} // namespace std

std::ostream &operator<<(std::ostream &os, const arkoi::il::Immediate &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::il::Variable &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::il::Memory &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::il::Operand &operand);

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
