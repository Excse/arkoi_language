#include "front/token.hpp"

#include <unordered_map>

#include "utils/utils.hpp"

using namespace arkoi::front;

std::optional<Token::Type> Token::lookup_keyword(const std::string_view &value) {
    static const std::unordered_map<std::string_view, Type> KEYWORDS = {
        {"if",     Type::If},
        {"else",   Type::Else},
        {"fun",    Type::Fun},
        {"return", Type::Return},
        {"u8",     Type::U8},
        {"s8",     Type::S8},
        {"u16",    Type::U16},
        {"s16",    Type::S16},
        {"u32",    Type::U32},
        {"s32",    Type::S32},
        {"u64",    Type::U64},
        {"s64",    Type::S64},
        {"usize",  Type::USize},
        {"ssize",  Type::SSize},
        {"f64",    Type::F64},
        {"f32",    Type::F32},
        {"bool",   Type::Bool},
        {"true",   Type::True},
        {"false",  Type::False},
    };

    const auto keyword = KEYWORDS.find(value);
    if (keyword != KEYWORDS.end()) {
        return {keyword->second};
    }

    return std::nullopt;
}

std::optional<Token::Type> Token::lookup_special_1(char value) {
    switch (value) {
        case '(': return Type::LParent;
        case ')': return Type::RParent;
        case '@': return Type::At;
        case ',': return Type::Comma;
        case '+': return Type::Plus;
        case '-': return Type::Minus;
        case '/': return Type::Slash;
        case '*': return Type::Asterisk;
        default: return std::nullopt;
    }
}

std::ostream &operator<<(std::ostream &os, const Token::Type &type) {
    switch (type) {
        case Token::Type::Indentation: return os << "Indentation";
        case Token::Type::Dedentation: return os << "Dedentation";
        case Token::Type::Newline: return os << "Newline";

        case Token::Type::Integer: return os << "Integer";
        case Token::Type::Floating: return os << "Floating";
        case Token::Type::Identifier: return os << "Identifier";
        case Token::Type::Comment: return os << "Comment";

        case Token::Type::If: return os << "if";
        case Token::Type::Else: return os << "else";
        case Token::Type::Fun: return os << "fun";
        case Token::Type::Return: return os << "return";
        case Token::Type::U8: return os << "u8";
        case Token::Type::S8: return os << "s8";
        case Token::Type::U16: return os << "u16";
        case Token::Type::S16: return os << "s16";
        case Token::Type::U32: return os << "u32";
        case Token::Type::S32: return os << "s32";
        case Token::Type::U64: return os << "u64";
        case Token::Type::S64: return os << "s64";
        case Token::Type::USize: return os << "usize";
        case Token::Type::SSize: return os << "ssize";
        case Token::Type::F32: return os << "f32";
        case Token::Type::F64: return os << "f64";
        case Token::Type::Bool: return os << "bool";
        case Token::Type::True: return os << "true";
        case Token::Type::False: return os << "false";

        case Token::Type::LParent: return os << "LParent";
        case Token::Type::RParent: return os << "RParent";
        case Token::Type::At: return os << "At";
        case Token::Type::Comma: return os << "Comma";
        case Token::Type::Plus: return os << "Plus";
        case Token::Type::Minus: return os << "Minus";
        case Token::Type::Slash: return os << "Slash";
        case Token::Type::Asterisk: return os << "Asterisk";

        case Token::Type::EndOfFile: return os << "EndOfFile";
        case Token::Type::Unknown: return os << "Unknown";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << to_string(token.type());
    os << "(contents=\"" << token.contents() << "\"";
    os << ", column=" << token.column();
    os << ", row=" << token.row() << ")";
    return os;
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
