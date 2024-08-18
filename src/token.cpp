#include "token.h"

std::string Token::type_name(Type type) {
    switch (type) {
        case Type::Number:
            return "NumberNode";
        case Type::Identifier:
            return "IdentifierNode";
        case Type::Comment:
            return "Comment";

        case Type::Fun:
            return "fun";
        case Type::Return:
            return "return";
        case Type::Bool:
            return "bool";
        case Type::U8:
            return "u8";
        case Type::S8:
            return "s8";
        case Type::U16:
            return "u16";
        case Type::S16:
            return "s16";
        case Type::U32:
            return "u32";
        case Type::S32:
            return "s32";
        case Type::U64:
            return "u64";
        case Type::S64:
            return "s64";
        case Type::USize:
            return "usize";
        case Type::SSize:
            return "ssize";

        case Type::LParent:
            return "(";
        case Type::RParent:
            return ")";
        case Type::LCBracket:
            return "{";
        case Type::RCBracket:
            return "}";
        case Type::At:
            return "@";
        case Type::Semicolon:
            return ";";
        case Type::Comma:
            return ",";

        case Type::EndOfFile:
            return "UnexpectedEndOfFile";
        case Type::Unknown:
            return "Unknown";
    }
}

std::optional<Token::Type> Token::lookup_keyword(const std::string_view &value) {
    static const std::unordered_map<std::string_view, Token::Type> KEYWORDS = {
            {"fun",    Token::Type::Fun},
            {"bool",   Token::Type::Bool},
            {"return", Token::Type::Return},
            {"u8",     Token::Type::U8},
            {"s8",     Token::Type::S8},
            {"u16",    Token::Type::U16},
            {"s16",    Token::Type::S16},
            {"u32",    Token::Type::U32},
            {"s32",    Token::Type::S32},
            {"u64",    Token::Type::U64},
            {"s64",    Token::Type::S64},
            {"usize",  Token::Type::USize},
            {"ssize",  Token::Type::SSize},
    };

    const auto keyword = KEYWORDS.find(value);
    if (keyword != KEYWORDS.end()) {
        return {keyword->second};
    }

    return std::nullopt;
}

std::optional<Token::Type> Token::lookup_special_1(char value) {
    switch (value) {
        case '(':
            return Token::Type::LParent;
        case ')':
            return Token::Type::RParent;
        case '{':
            return Token::Type::LCBracket;
        case '}':
            return Token::Type::RCBracket;
        case '@':
            return Token::Type::At;
        case ';':
            return Token::Type::Semicolon;
        case ',':
            return Token::Type::Comma;
        default:
            return std::nullopt;
    }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << Token::type_name(token.type());
    os << ", value: " << token.value();
    os << ", column: " << token.column();
    os << ", row: " << token.row();
    return os;
}
