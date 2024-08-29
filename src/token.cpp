#include "token.h"

#include "utils.h"

std::optional<Token::Type> Token::lookup_keyword(const std::string_view &value) {
    static const std::unordered_map<std::string_view, Token::Type> KEYWORDS = {
            {"fun",    Token::Type::Fun},
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
        case '+':
            return Token::Type::Plus;
        case '-':
            return Token::Type::Minus;
        case '/':
            return Token::Type::Slash;
        case '*':
            return Token::Type::Asterisk;
        default:
            return std::nullopt;
    }
}


std::ostream &operator<<(std::ostream &os, const Token::Type &type) {
    switch (type) {
        case Token::Type::Number:
            return os << "NumberNode";
        case Token::Type::Identifier:
            return os << "IdentifierNode";
        case Token::Type::Comment:
            return os << "Comment";

        case Token::Type::Fun:
            return os << "fun";
        case Token::Type::Return:
            return os << "return";
        case Token::Type::U8:
            return os << "u8";
        case Token::Type::S8:
            return os << "s8";
        case Token::Type::U16:
            return os << "u16";
        case Token::Type::S16:
            return os << "s16";
        case Token::Type::U32:
            return os << "u32";
        case Token::Type::S32:
            return os << "s32";
        case Token::Type::U64:
            return os << "u64";
        case Token::Type::S64:
            return os << "s64";
        case Token::Type::USize:
            return os << "usize";
        case Token::Type::SSize:
            return os << "ssize";

        case Token::Type::LParent:
            return os << "(";
        case Token::Type::RParent:
            return os << ")";
        case Token::Type::LCBracket:
            return os << "{";
        case Token::Type::RCBracket:
            return os << "}";
        case Token::Type::At:
            return os << "@";
        case Token::Type::Semicolon:
            return os << ";";
        case Token::Type::Comma:
            return os << ",";
        case Token::Type::Plus:
            return os << "+";
        case Token::Type::Minus:
            return os << "-";
        case Token::Type::Slash:
            return os << "/";
        case Token::Type::Asterisk:
            return os << "*";

        case Token::Type::EndOfFile:
            return os << "UnexpectedEndOfFile";
        case Token::Type::Unknown:
            return os << "Unknown";
        default:
            throw std::runtime_error("Token type not implemented.");
    }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << to_string(token.type());
    os << ", value: " << token.value();
    os << ", column: " << token.column();
    os << ", row: " << token.row();
    return os;
}
