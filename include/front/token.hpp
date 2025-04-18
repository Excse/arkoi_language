#pragma once

#include <iostream>
#include <optional>
#include <utility>

namespace arkoi::front {

class Token {
public:
    enum class Type {
        Indentation,
        Dedentation,
        Newline,

        Integer,
        Floating,
        Identifier,
        Comment,

        // Keyword
        If,
        Else,
        Fun,
        Return,
        U8,
        S8,
        U16,
        S16,
        U32,
        S32,
        U64,
        S64,
        USize,
        SSize,
        F32,
        F64,
        Bool,
        True,
        False,

        LParent,
        RParent,
        At,
        Comma,
        Plus,
        Minus,
        Slash,
        Asterisk,
        GreaterThan,
        LessThan,
        Equal,
        Colon,

        EndOfFile,
        Unknown,
    };

public:
    Token(Type type, size_t column, size_t row, std::string contents)
        : _contents(std::move(contents)), _column(column), _row(row), _type(type) {}

    [[nodiscard]] auto &contents() const { return _contents; }

    [[nodiscard]] auto column() const { return _column; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto row() const { return _row; }

    [[nodiscard]] static std::optional<Type> lookup_keyword(const std::string_view &value);

    [[nodiscard]] static std::optional<Type> lookup_special(char value);

private:
    std::string _contents;
    size_t _column, _row;
    Type _type;
};

} // namespace arkoi::front

std::ostream &operator<<(std::ostream &os, const arkoi::front::Token::Type &type);

std::ostream &operator<<(std::ostream &os, const arkoi::front::Token &token);

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
