#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "sem/symbol.hpp"

namespace arkoi::sem {

class SymbolTable {
public:
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _parent(std::move(parent)) {}

    template<typename Type, typename... Args>
    std::shared_ptr<Symbol> &insert(const std::string &name, Args &&... args);

    template<typename... Types>
    [[nodiscard]] std::shared_ptr<Symbol> &lookup(const std::string &name);

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols{};
    std::shared_ptr<SymbolTable> _parent;
};

class IdentifierAlreadyTaken final : public std::runtime_error {
public:
    explicit IdentifierAlreadyTaken(const std::string &name)
        : std::runtime_error("The identifier " + name + " is already taken.") {}
};

class IdentifierNotFound final : public std::runtime_error {
public:
    explicit IdentifierNotFound(const std::string &name)
        : std::runtime_error("The identifier " + name + " was not found.") {}
};

#include "../../src/sem/symbol_table.tpp"

} // namespace arkoi::sem

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
