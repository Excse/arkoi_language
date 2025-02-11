#pragma once

#include <functional>
#include <utility>
#include <string>
#include <vector>

#include "token.hpp"

namespace arkoi::front {

class Scanner {
private:
    struct Location {
        size_t column, row;
    };

public:
    Scanner(std::string data) : _data(std::move(data)) {}

    [[nodiscard]] std::vector<Token> tokenize();

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    [[nodiscard]] Token _next_token();

    [[nodiscard]] Token _lex_comment();

    [[nodiscard]] Token _lex_identifier();

    [[nodiscard]] Token _lex_number();

    [[nodiscard]] Token _lex_char();

    [[nodiscard]] Token _lex_special();

    [[nodiscard]] std::string _current_view();

    [[nodiscard]] char _current_char();

    [[nodiscard]] bool _is_eol();

    [[nodiscard]] Location _mark_start();

    void _next();

    char _peek();

    void _consume(char expected);

    char _consume(const std::function<bool(char)> &predicate, const std::string &expected);

    [[nodiscard]] bool _try_consume(char expected);

    [[nodiscard]] std::optional<char> _try_consume(const std::function<bool(char)> &predicate);

    [[nodiscard]] static size_t _leading_spaces(const std::string &line);

    [[nodiscard]] static bool _is_digit(char input);

    [[nodiscard]] static bool _is_ident_start(char input);

    [[nodiscard]] static bool _is_ident_inner(char input);

    [[nodiscard]] static bool _is_not_newline(char input);

    [[nodiscard]] static bool _is_ascii(char input);

    [[nodiscard]] static bool _is_space(char input);

    [[nodiscard]] static bool _is_hex(char input);

    [[nodiscard]] static bool _is_hex_expo(char input);

    [[nodiscard]] static bool _is_expo(char input);

    [[nodiscard]] static bool _is_decimal_sign(char input);

private:
    size_t _start{}, _row{}, _column{}, _indentation{};
    std::string_view _current_line;
    std::string _data;
    bool _failed{};
};

class ScannerError : public std::runtime_error {
public:
    ScannerError(const std::string &error) : std::runtime_error(error) {}
};

class UnexpectedEndOfLine : public ScannerError {
public:
    UnexpectedEndOfLine() : ScannerError("Unexpectedly reached the End Of Line") {}
};

class UnexpectedChar : public ScannerError {
public:
    UnexpectedChar(const std::string &expected, char got)
        : ScannerError("Expected " + expected + " but got " + std::string(1, got)) {}
};

class UnknownChar : public ScannerError {
public:
    UnknownChar(char got) : ScannerError("Didn't expect " + std::string(1, got)) {}
};

class NumberOutOfRange : public ScannerError {
public:
    NumberOutOfRange(const std::string &number)
        : ScannerError("The number " + number + " exceeds the 64bit limitations.") {}
};

} // namespace arkoi::front

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
