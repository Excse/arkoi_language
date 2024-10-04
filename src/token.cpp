#include "token.hpp"

#include "utils.hpp"

std::optional<Token::Type> Token::lookup_keyword(const std::string_view &value) {
    static const std::unordered_map<std::string_view, Type> KEYWORDS = {
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

        case Token::Type::LParent: return os << "(";
        case Token::Type::RParent: return os << ")";
        case Token::Type::At: return os << "@";
        case Token::Type::Comma: return os << ",";
        case Token::Type::Plus: return os << "+";
        case Token::Type::Minus: return os << "-";
        case Token::Type::Slash: return os << "/";
        case Token::Type::Asterisk: return os << "*";

        case Token::Type::EndOfFile: return os << "UnexpectedEndOfLine";
        case Token::Type::Unknown: return os << "Unknown";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << to_string(token.type());
    os << ", contents: " << token.contents();
    os << ", column: " << token.column();
    os << ", row: " << token.row();
    return os;
}
