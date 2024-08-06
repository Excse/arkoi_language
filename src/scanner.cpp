#include "scanner.h"

std::vector<Token> Scanner::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        try {
            Token token = _next_token();
            tokens.push_back(token);

            if (token.type() == Token::Type::EndOfFile) {
                break;
            }
        } catch (const UnexpectedEndOfFile &error) {
            std::cerr << error.what() << std::endl;
            break;
        } catch (const UnexpectedChar &error) {
            std::cerr << error.what() << std::endl;
            _position += 1;
        }
    }

    return tokens;
}

Token Scanner::_next_token() {
    while (_try_consume(_is_space));

    if (_position >= _data.size()) {
        return Token{Token::Type::EndOfFile};
    }

    char current = _current();
    if (std::isalpha(current)) {
        return _lex_identifier();
    } else if (current == '\'' || std::isdigit(current)) {
        return _lex_number();
    } else if (current == '#') {
        return _lex_comment();
    }

    return _lex_special();
}

Token Scanner::_lex_comment() {
    size_t start = _position;

    _consume('#');
    while (_try_consume(_is_not_newline));

    std::string_view value = _data.substr(start, _position - start - 1);
    return Token{Token::Type::Comment, value};
}

Token Scanner::_lex_identifier() {
    size_t start = _position;

    _consume(_is_ident_start, "Expected _, a-z or A-Z");
    while (_try_consume(_is_ident));

    std::string_view value = _data.substr(start, _position - start);
    if (auto keyword = Token::lookup_keyword(value)) {
        return Token{*keyword};
    }

    return Token{Token::Type::Identifier, value};
}

Token Scanner::_lex_number() {
    size_t start = _position;

    _consume(_is_digit, "Expected 0-9");

    if (_try_consume('x')) {
        _consume(_is_hex, "Expected 0-9, a-f or A-F");
        while (_try_consume(_is_hex));
    } else {
        while (_try_consume(_is_digit));
    }

    std::string_view value = _data.substr(start, _position - start);
    return Token(Token::Type::Number, value);
}

Token Scanner::_lex_special() {
    char current = _current();
    _position += 1;

    if (auto special = Token::lookup_special_1(current)) {
        return Token{*special};
    }

    return Token{Token::Type::Unknown};
}


void Scanner::_consume(char expected) {
    _consume([&](char input) { return input == expected; }, "Expected " + std::string(1, expected));
}

bool Scanner::_try_consume(char expected) {
    try {
        _consume(expected);
        return true;
    } catch (...) {
        return false;
    }
}

void Scanner::_consume(const std::function<bool(char)> &predicate, const std::string &error) {
    char current = _current();
    if (_position >= _data.size()) {
        throw UnexpectedEndOfFile();
    }

    if (!predicate(current)) {
        throw UnexpectedChar(error);
    }

    _position += 1;
}

bool Scanner::_try_consume(const std::function<bool(char)> &predicate) {
    try {
        _consume(predicate, "");
        return true;
    } catch (...) {
        return false;
    }
}

bool Scanner::_is_digit(char input) {
    return std::isdigit(static_cast<unsigned char>(input));
}

bool Scanner::_is_ident_start(char input) {
    return std::isalpha(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_ident(char input) {
    return std::isalnum(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_not_newline(char input) {
    return input != '\n';
}

bool Scanner::_is_space(char input) {
    return std::isspace(static_cast<unsigned char>(input));
}

bool Scanner::_is_hex(char input) {
    return (input >= '0' && input <= '9') ||
           (input >= 'a' && input <= 'f') ||
           (input >= 'A' && input <= 'F');
}
