#pragma once

#include <unordered_map>
#include <iostream>
#include <optional>
#include <cstdint>
#include <utility>

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

    friend std::ostream &operator<<(std::ostream &os, const Token &token);

    friend std::ostream &operator<<(std::ostream &os, const Type &type);

private:
    std::string _contents;
    size_t _column, _row;
    Type _type;
};