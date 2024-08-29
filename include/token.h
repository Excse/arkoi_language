#ifndef ARKOI_LANGUAGE_TOKEN_H
#define ARKOI_LANGUAGE_TOKEN_H

#include <unordered_map>
#include <iostream>
#include <optional>
#include <variant>
#include <cstdint>

class Token {
public:
    enum class Type {
        Number,
        Identifier,
        Comment,

        // Keyword
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

        // Special 1
        LParent,
        RParent,
        LCBracket,
        RCBracket,
        At,
        Semicolon,
        Comma,
        Plus,
        Minus,
        Slash,
        Asterisk,

        EndOfFile,
        Unknown,
    };

public:
    Token(Type type, size_t column, size_t row, std::string_view value)
            : _value(value), _column(column), _row(row), _type(type) {}

    [[nodiscard]] const auto &value() const { return _value; }

    [[nodiscard]] const auto &type() const { return _type; }

    [[nodiscard]] auto column() const { return _column; }

    [[nodiscard]] auto row() const { return _row; }

    [[nodiscard]] static std::optional<Token::Type> lookup_keyword(const std::string_view &value);

    [[nodiscard]] static std::optional<Token::Type> lookup_special_1(char value);

private:
    std::string_view _value;
    size_t _column, _row;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const Token::Type &type);

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif //ARKOI_LANGUAGE_TOKEN_H
