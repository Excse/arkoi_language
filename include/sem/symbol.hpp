#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "sem/type.hpp"

struct Symbol;

namespace arkoi::sem {

class Variable;

class Function {
public:
    explicit Function(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] auto &parameters() const { return _parameters; }
    void set_parameters(std::vector<std::shared_ptr<Variable>> &&symbols) { _parameters = std::move(symbols); }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &return_type() const { return _return_type.value(); }
    void set_return_type(Type type) { _return_type = type; }

private:
    std::vector<std::shared_ptr<Variable>> _parameters{};
    std::optional<Type> _return_type{};
    std::string _name;
};

class Variable {
public:
    Variable(std::string name, Type type) : _type(type), _name(std::move(name)) {}

    explicit Variable(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] auto &type() const { return _type.value(); }
    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::optional<Type> _type{};
    std::string _name;
};

} // namespace arkoi::sem

struct Symbol : std::variant<arkoi::sem::Function, arkoi::sem::Variable> {
    using variant::variant;
};

std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Symbol> &symbol);

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
