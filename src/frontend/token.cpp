#include "frontend/token.hpp"

#include "utils/utils.hpp"

std::optional<Token::Type> Token::lookup_keyword(const std::string_view &value) {
    static const std::unordered_map<std::string_view, Type> KEYWORDS = {
            {"if",    Type::If},
            {"else",    Type::Else},
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
        using Type = Token::Type;
        case Type::Indentation: return os << "Indentation";
        case Type::Dedentation: return os << "Dedentation";
        case Type::Newline: return os << "Newline";

        case Type::Integer: return os << "Integer";
        case Type::Floating: return os << "Floating";
        case Type::Identifier: return os << "Identifier";
        case Type::Comment: return os << "Comment";

        case Type::If: return os << "if";
        case Type::Else: return os << "else";
        case Type::Fun: return os << "fun";
        case Type::Return: return os << "return";
        case Type::U8: return os << "u8";
        case Type::S8: return os << "s8";
        case Type::U16: return os << "u16";
        case Type::S16: return os << "s16";
        case Type::U32: return os << "u32";
        case Type::S32: return os << "s32";
        case Type::U64: return os << "u64";
        case Type::S64: return os << "s64";
        case Type::USize: return os << "usize";
        case Type::SSize: return os << "ssize";
        case Type::F32: return os << "f32";
        case Type::F64: return os << "f64";
        case Type::Bool: return os << "bool";
        case Type::True: return os << "true";
        case Type::False: return os << "false";

        case Type::LParent: return os << "(";
        case Type::RParent: return os << ")";
        case Type::At: return os << "@";
        case Type::Comma: return os << ",";
        case Type::Plus: return os << "+";
        case Type::Minus: return os << "-";
        case Type::Slash: return os << "/";
        case Type::Asterisk: return os << "*";

        case Type::EndOfFile: return os << "UnexpectedEndOfLine";
        case Type::Unknown: return os << "Unknown";
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