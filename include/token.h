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
        Bool,
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

        EndOfFile,
        Unknown,
    };

public:
    explicit Token(Type type, size_t column, size_t row, std::string_view value)
            : _value(value), _column(column), _row(row), _type(type) {}

    [[nodiscard]] const size_t &column() const { return _column; }

    [[nodiscard]] const size_t &row() const { return _row; }

    [[nodiscard]] const std::string_view &value() const { return _value; }

    [[nodiscard]] const Type &type() const { return _type; }

    [[nodiscard]] std::string name() const;

    static std::optional<Token::Type> lookup_keyword(const std::string_view &value);

    static std::optional<Token::Type> lookup_special_1(char value);

private:
    std::string_view _value;
    size_t _column, _row;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif //ARKOI_LANGUAGE_TOKEN_H
