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
        } catch (const UnknownChar &error) {
            std::cerr << error.what() << std::endl;
            _next();
        }
    }

    return tokens;
}

Token Scanner::_next_token() {
    while (_try_consume(_is_space));

    Location start = _current_location();
    if (_is_eof()) {
        return Token{Token::Type::EndOfFile, start.column, start.row, ""};
    }

    char current = _current_char();
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
    Location start = _current_location();
    _mark_start();

    _consume('#');
    while (_try_consume(_is_not_newline));

    return Token{Token::Type::Comment, start.column, start.row, _view()};
}

Token Scanner::_lex_identifier() {
    Location start = _current_location();
    _mark_start();

    _consume(_is_ident_start, "_, a-z or A-Z");
    while (_try_consume(_is_ident));

    std::string_view value = _view();
    if (auto keyword = Token::lookup_keyword(value)) {
        return Token{*keyword, start.column, start.row, value};
    }

    return Token{Token::Type::Identifier, start.column, start.row, value};
}

Token Scanner::_lex_number() {
    Location start = _current_location();
    _mark_start();

    _consume(_is_digit, "0-9");

    if (_try_consume('x')) {
        _consume(_is_hex, "0-9, a-f or A-F");
        while (_try_consume(_is_hex));
    } else {
        while (_try_consume(_is_digit));
    }

    return Token(Token::Type::Number, start.column, start.row, _view());
}

Token Scanner::_lex_special() {
    Location start = _current_location();
    _mark_start();

    char current = _current_char();
    _next();

    if (auto special = Token::lookup_special_1(current)) {
        return Token{*special, start.column, start.row, _view()};
    }

    throw UnknownChar(current);
}

char Scanner::_current_char() {
    return _data[_position];
}

bool Scanner::_is_eof() {
    return _position >= _data.size();
}

Location Scanner::_current_location() {
    return Location{_column, _row};
}

void Scanner::_mark_start() {
    _start = _position;
}

std::string_view Scanner::_view() {
    return _data.substr(_start, (_position - _start));
}

void Scanner::_next() {
    if (_current_char() == '\n') {
        _column++;
        _row = 0;
    } else {
        _row++;
    }

    _position += 1;
}

void Scanner::_consume(char expected) {
    _consume([&](char input) { return input == expected; }, std::string(1, expected));
}

bool Scanner::_try_consume(char expected) {
    try {
        _consume(expected);
        return true;
    } catch (...) {
        return false;
    }
}

char Scanner::_consume(const std::function<bool(char)> &predicate, const std::string &expected) {
    char current = _current_char();
    if (_position >= _data.size()) {
        throw UnexpectedEndOfFile();
    }

    if (!predicate(current)) {
        throw UnexpectedChar(expected, current);
    }

    _next();

    return current;
}

std::optional<char> Scanner::_try_consume(const std::function<bool(char)> &predicate) {
    try {
        char consumed = _consume(predicate, "");
        return consumed;
    } catch (...) {
        return std::nullopt;
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