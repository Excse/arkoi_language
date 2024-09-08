#include "scanner.h"

std::vector<Token> Scanner::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        try {
            auto token = _next_token();
            tokens.push_back(token);

            if (token.type() == Token::Type::EndOfFile) {
                break;
            }
        } catch (const UnexpectedEndOfFile &error) {
            std::cerr << error.what() << std::endl;
            _failed = true;
            break;
        } catch (const UnexpectedChar &error) {
            std::cerr << error.what() << std::endl;
            _failed = true;
            _next();
        } catch (const UnknownChar &error) {
            std::cerr << error.what() << std::endl;
            _failed = true;
            _next();
        }
    }

    return tokens;
}

Token Scanner::_next_token() {
    while (_try_consume(_is_space));

    Location start = _mark_start();
    if (_is_eof()) {
        return {Token::Type::EndOfFile, start.column, start.row, ""};
    }

    auto current = _current_char();
    if (_is_ident_start(current)) {
        return _lex_identifier();
    } else if (current == '-' || _is_digit(current)) {
        return _lex_number();
    } else if (current == '\'') {
        return _lex_char();
    } else if (current == '#') {
        return _lex_comment();
    }

    return _lex_special();
}

Token Scanner::_lex_comment() {
    Location start = _mark_start();

    _consume('#');
    while (_try_consume(_is_not_newline));

    return {Token::Type::Comment, start.column, start.row, _current_view()};
}

Token Scanner::_lex_identifier() {
    Location start = _mark_start();

    _consume(_is_ident_start, "_, a-z or A-Z");
    while (_try_consume(_is_ident_inner));

    auto value = _current_view();
    if (auto keyword = Token::lookup_keyword(value)) {
        return {*keyword, start.column, start.row, value};
    }

    return {Token::Type::Identifier, start.column, start.row, value};
}

Token Scanner::_lex_number() {
    Location start = _mark_start();
    if (_try_consume('-') && !_is_digit(_peek())) {
        return {Token::Type::Minus, start.column, start.row, _current_view()};
    }

    auto consumed = _consume(_is_digit, "0-9");
    if (consumed == '0' && _try_consume('x')) {
        _consume(_is_hex_start, "0-9, a-f or A-F");
        while (_try_consume(_is_hex_inner));
    } else {
        while (_try_consume(_is_digit));
    }

    auto number = std::string(_current_view());
    try {
        if (number.starts_with("-")) {
            std::stoll(number);
        } else {
            std::stoull(number);
        }
    } catch (const std::out_of_range &error) {
        throw NumberOutOfRange(number);
    }

    return {Token::Type::Number, start.column, start.row, _current_view()};
}

Token Scanner::_lex_char() {
    Location start = _mark_start();

    _consume('\'');
    auto consumed = _consume(_is_ascii, "'");
    _consume('\'');

    return {Token::Type::Number, start.column, start.row, std::to_string(consumed)};
}

Token Scanner::_lex_special() {
    Location start = _mark_start();

    auto current = _current_char();
    if (auto special = Token::lookup_special_1(current)) {
        _next();
        return {*special, start.column, start.row, _current_view()};
    }

    throw UnknownChar(current);
}

char Scanner::_current_char() {
    return _data[_position];
}

bool Scanner::_is_eof() {
    return _position >= _data.size();
}

Scanner::Location Scanner::_mark_start() {
    _start = _position;
    return Location{_column, _row};
}

std::string Scanner::_current_view() {
    return std::string(_data.substr(_start, (_position - _start)));
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

char Scanner::_peek() {
    if (_position >= _data.size()) return 0;
    return _data[_position];
}

void Scanner::_consume(char expected) {
    _consume([&](char input) { return input == expected; }, std::string(1, expected));
}

bool Scanner::_try_consume(char expected) {
    try {
        _consume(expected);
        return true;
    } catch (const ScannerError &) {
        return false;
    }
}

char Scanner::_consume(const std::function<bool(char)> &predicate, const std::string &expected) {
    auto current = _current_char();
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
        auto consumed = _consume(predicate, "");
        return consumed;
    } catch (const ScannerError &) {
        return std::nullopt;
    }
}

bool Scanner::_is_digit(char input) {
    return std::isdigit(static_cast<unsigned char>(input));
}

bool Scanner::_is_ident_start(char input) {
    return std::isalpha(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_ident_inner(char input) {
    return std::isalnum(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_not_newline(char input) {
    return input != '\n';
}

bool Scanner::_is_ascii(char input) {
    return static_cast<unsigned char>(input) <= 127;
}

bool Scanner::_is_space(char input) {
    return std::isspace(static_cast<unsigned char>(input));
}

bool Scanner::_is_hex_start(char input) {
    return (input >= '0' && input <= '9') ||
           (input >= 'a' && input <= 'f') ||
           (input >= 'A' && input <= 'F');
}

bool Scanner::_is_hex_inner(char input) {
    return _is_hex_start(input) || input == '_';
}
