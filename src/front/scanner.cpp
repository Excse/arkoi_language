#include "front/scanner.hpp"

#include <sstream>

using namespace arkoi::front;
using namespace arkoi;

static constexpr size_t SPACE_INDENTATION = 4;

std::vector<Token> Scanner::tokenize() {
    std::vector<Token> tokens;

    std::istringstream stream(_data);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        _current_line = line;

        auto leading_spaces = _leading_spaces(line);
        if (leading_spaces % SPACE_INDENTATION != 0) {
            std::cerr << "Leading spaces are not of a multiple of 4" << std::endl;
            _failed = true;
            continue;
        }

        while (leading_spaces > _indentation) {
            tokens.emplace_back(Token::Type::Indentation, _column, _row, "");
            _indentation += SPACE_INDENTATION;
            _column += SPACE_INDENTATION;
        }

        while (leading_spaces < _indentation) {
            tokens.emplace_back(Token::Type::Dedentation, _column, _row, "");
            _indentation -= SPACE_INDENTATION;
            _column -= SPACE_INDENTATION;
        }

        while (!_is_eol()) {
            try {
                auto token = _next_token();
                tokens.push_back(token);
            } catch (const UnexpectedEndOfLine &error) {
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

        tokens.emplace_back(Token::Type::Newline, _column, _row, "");

        _column = _indentation;
        _row++;
    }

    while (_indentation) {
        tokens.emplace_back(Token::Type::Dedentation, _column, _row, "");
        _indentation -= SPACE_INDENTATION;
    }

    tokens.emplace_back(Token::Type::EndOfFile, 0, 0, "");

    return tokens;
}

Token Scanner::_next_token() {
    while (_try_consume(_is_space));

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
    bool floating;

    if (consumed == '0' && _try_consume('x')) {
        _consume(_is_hex, "0-9, a-f or A-F");

        while (_try_consume(_is_hex));

        floating = _try_consume('.');

        while (_try_consume(_is_hex));

        if (_try_consume(_is_hex_expo)) {
            std::ignore = _try_consume(_is_decimal_sign);

            while (_try_consume(_is_hex));
        }
    } else {
        while (_try_consume(_is_digit));

        floating = _try_consume('.');

        while (_try_consume(_is_digit));

        if (_try_consume(_is_expo)) {
            floating = true;

            std::ignore = _try_consume(_is_decimal_sign);

            while (_try_consume(_is_hex));
        }
    }

    auto number = _current_view();
    try {
        if (floating) {
            std::stold(number);
        } else if (number.starts_with("-")) {
            std::stoll(number);
        } else {
            std::stoull(number);
        }
    } catch (const std::out_of_range &error) {
        throw NumberOutOfRange(number);
    }

    if (floating) {
        return {Token::Type::Floating, start.column, start.row, number};
    } else {
        return {Token::Type::Integer, start.column, start.row, number};
    }
}

Token Scanner::_lex_char() {
    Location start = _mark_start();

    _consume('\'');
    auto consumed = _consume(_is_ascii, "'");
    _consume('\'');

    return {Token::Type::Integer, start.column, start.row, std::to_string(consumed)};
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
    return _current_line[_column];
}

bool Scanner::_is_eol() {
    return _column >= _current_line.size();
}

Scanner::Location Scanner::_mark_start() {
    _start = _column;
    return Location{_column, _row};
}

std::string Scanner::_current_view() {
    return std::string(_current_line.substr(_start, (_column - _start)));
}

void Scanner::_next() {
    _column += 1;
}

char Scanner::_peek() {
    if (_column >= _current_line.size()) return 0;
    return _current_line[_column];
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
    if (_column >= _current_line.size()) {
        throw UnexpectedEndOfLine();
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

size_t Scanner::_leading_spaces(const std::string &line) {
    size_t count = 0;

    for (const auto &current: line) {
        if (current != ' ') break;
        count++;
    }

    return count;
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

bool Scanner::_is_hex(char input) {
    return (input >= '0' && input <= '9') ||
           (input >= 'a' && input <= 'f') ||
           (input >= 'A' && input <= 'F');
}

bool Scanner::_is_hex_expo(char input) {
    return input == 'p' || input == 'P';
}

bool Scanner::_is_expo(char input) {
    return input == 'e' || input == 'E';
}

bool Scanner::_is_decimal_sign(char input) {
    return input == '+' || input == '-';
}
