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
            _next();
        }
    }

    return tokens;
}

Token Scanner::_next_token() {
    if (_is_eof()) {
        return Token{Token::Type::EndOfFile};
    }

    while (_try_consume(_is_space));

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
    _mark_start();

    _consume('#');
    while (_try_consume(_is_not_newline));

    return Token{Token::Type::Comment, _view()};
}

Token Scanner::_lex_identifier() {
    _mark_start();

    _consume(_is_ident_start, "Expected _, a-z or A-Z");
    while (_try_consume(_is_ident));

    std::string_view value = _view();
    if (auto keyword = Token::lookup_keyword(value)) {
        return Token{*keyword};
    }

    return Token{Token::Type::Identifier, value};
}

Token Scanner::_lex_number() {
    _mark_start();

    _consume(_is_digit, "Expected 0-9");

    if (_try_consume('x')) {
        _consume(_is_hex, "Expected 0-9, a-f or A-F");
        while (_try_consume(_is_hex));
    } else {
        while (_try_consume(_is_digit));
    }

    return Token(Token::Type::Number, _view());
}

Token Scanner::_lex_special() {
    char current = _current();
    _next();

    if (auto special = Token::lookup_special_1(current)) {
        return Token{*special};
    }

    throw UnexpectedChar("Didn't expect " + std::string(1, current));
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
