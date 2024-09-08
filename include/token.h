#ifndef ARKOI_LANGUAGE_TOKEN_H
#define ARKOI_LANGUAGE_TOKEN_H

#include <unordered_map>
#include <iostream>
#include <optional>
#include <cstdint>
#include <utility>

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
    Token(Type type, int64_t column, int64_t row, std::string value)
            : _column(column), _row(row), _value(std::move(value)), _type(type) {}

    [[nodiscard]] auto &value() const { return _value; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto column() const { return _column; }

    [[nodiscard]] auto row() const { return _row; }

    [[nodiscard]] static std::optional<Token::Type> lookup_keyword(const std::string_view &value);

    [[nodiscard]] static std::optional<Token::Type> lookup_special_1(char value);

    friend std::ostream &operator<<(std::ostream &os, const Token &token);

    friend std::ostream &operator<<(std::ostream &os, const Type &type);

private:
    int64_t _column, _row;
    std::string _value;
    Type _type;
};

#endif //ARKOI_LANGUAGE_TOKEN_H
