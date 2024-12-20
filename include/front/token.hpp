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

        // Special 1
        LParent,
        RParent,
        At,
        Comma,
        Plus,
        Minus,
        Slash,
        Asterisk,

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

    [[nodiscard]] static std::optional<Token::Type> lookup_keyword(const std::string_view &value);

    [[nodiscard]] static std::optional<Token::Type> lookup_special_1(char value);

private:
    std::string _contents;
    size_t _column, _row;
    Type _type;
};

} // namespace arkoi::front

std::ostream &operator<<(std::ostream &os, const arkoi::front::Token &token);

std::ostream &operator<<(std::ostream &os, const arkoi::front::Token::Type &type);