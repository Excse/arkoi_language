#ifndef SPL_TOKEN_H
#define SPL_TOKEN_H

#include <unordered_map>
#include <iostream>
#include <optional>
#include <variant>
#include <cstdint>

class Token {
public:
    using Value = std::optional<std::string_view>;

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
    explicit Token(Type type, Value value = std::nullopt) : _value(value), _type(type) {}

    [[nodiscard]] const Value &value() const { return _value; }

    [[nodiscard]] const Type &type() const { return _type; }

    [[nodiscard]] std::string name() const;

    static std::optional<Token::Type> lookup_keyword(const std::string_view &value);

    static std::optional<Token::Type> lookup_special_1(char value);

private:
    Value _value;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const Token &token);

#endif //SPL_TOKEN_H
