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

        const auto leading_spaces = _leading_spaces(line);
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
    while (_try_consume(_is_space)) {}

    const auto current = _current_char();
    if (_is_ident_start(current)) {
        return _lex_identifier();
    }

    if (current == '-' || _is_digit(current)) {
        return _lex_number();
    }

    if (current == '\'') {
        return _lex_char();
    }

    if (current == '#') {
        return _lex_comment();
    }

    return _lex_special();
}

Token Scanner::_lex_comment() {
    auto[column, row] = _mark_start();

    _consume('#');
    while (_try_consume(_is_not_newline)) {
    }

    return {Token::Type::Comment, column, row, _current_view()};
}

Token Scanner::_lex_identifier() {
    auto[column, row] = _mark_start();

    _consume(_is_ident_start, "_, a-z or A-Z");
    while (_try_consume(_is_ident_inner)) {
    }

    auto value = _current_view();
    if (auto keyword = Token::lookup_keyword(value)) {
        return {*keyword, column, row, value};
    }

    return {Token::Type::Identifier, column, row, value};
}

Token Scanner::_lex_number() {
    auto[column, row] = _mark_start();

    if (_try_consume('-') && !_is_digit(_peek())) {
        return {Token::Type::Minus, column, row, _current_view()};
    }

    const auto consumed = _consume(_is_digit, "0-9");
    bool floating;

    if (consumed == '0' && _try_consume('x')) {
        _consume(_is_hex, "0-9, a-f or A-F");

        while (_try_consume(_is_hex)) {
        }

        floating = _try_consume('.');

        while (_try_consume(_is_hex)) {
        }

        if (_try_consume(_is_hex_expo)) {
            std::ignore = _try_consume(_is_decimal_sign);

            while (_try_consume(_is_hex));
        }
    } else {
        while (_try_consume(_is_digit)) {
        }

        floating = _try_consume('.');

        while (_try_consume(_is_digit)) {
        }

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
    } catch (const std::out_of_range &) {
        throw NumberOutOfRange(number);
    }

    auto kind = (floating ? Token::Type::Floating : Token::Type::Integer);

    return {kind, column, row, number};
}

Token Scanner::_lex_char() {
    auto[column, row] = _mark_start();

    _consume('\'');
    const auto consumed = _consume(_is_ascii, "'");
    _consume('\'');

    return {Token::Type::Integer, column, row, std::to_string(consumed)};
}

Token Scanner::_lex_special() {
    auto[column, row] = _mark_start();

    const auto current = _current_char();
    if (auto special = Token::lookup_special(current)) {
        _next();
        return {*special, column, row, _current_view()};
    }

    throw UnknownChar(current);
}

char Scanner::_current_char() const {
    return _current_line[_column];
}

bool Scanner::_is_eol() const {
    return _column >= _current_line.size();
}

Scanner::Location Scanner::_mark_start() {
    _start = _column;
    return Location{_column, _row};
}

std::string Scanner::_current_view() const {
    return std::string(_current_line.substr(_start, (_column - _start)));
}

void Scanner::_next() {
    _column += 1;
}

char Scanner::_peek() const {
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
    const auto current = _current_char();
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

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
