#ifndef ARKOI_LANGUAGE_SCANNER_H
#define ARKOI_LANGUAGE_SCANNER_H

#include <functional>
#include <string>
#include <vector>

#include "token.h"

class UnexpectedEndOfFile : public std::runtime_error {
public:
    UnexpectedEndOfFile() : std::runtime_error("Unexpectedly reached the End Of File") {}
};

class UnexpectedChar : public std::runtime_error {
public:
    explicit UnexpectedChar(const std::string &expected, char got)
            : std::runtime_error("Expected " + expected + " but got " + std::string(1, got)) {}
};

class UnknownChar : public std::runtime_error {
public:
    explicit UnknownChar(char got) : std::runtime_error("Didn't expect " + std::string(1, got)) {}
};

struct Location {
    size_t column, row;
};

class Scanner {
public:
    explicit Scanner(std::string_view data)
            : _position(0), _start(0), _data(data), _column(0), _row(0) {}

    std::vector<Token> tokenize();

private:
    Token _next_token();

    Token _lex_comment();

    Token _lex_identifier();

    Token _lex_number();

    Token _lex_special();

    [[nodiscard]] Location _current_location();

    [[nodiscard]] std::string_view _view();

    [[nodiscard]] char _current_char();

    [[nodiscard]] bool _is_eof();

    void _mark_start();

    void _next();

    void _consume(char expected);

    char _consume(const std::function<bool(char)> &predicate, const std::string &expected);

    [[nodiscard]] bool _try_consume(char expected);

    [[nodiscard]] std::optional<char> _try_consume(const std::function<bool(char)> &predicate);

    [[nodiscard]] static bool _is_digit(char input);

    [[nodiscard]] static bool _is_ident_start(char input);

    [[nodiscard]] static bool _is_ident(char input);

    [[nodiscard]] static bool _is_not_newline(char input);

    [[nodiscard]] static bool _is_space(char input);

    [[nodiscard]] static bool _is_hex(char input);

private:
    size_t _position, _start;
    std::string_view _data;
    size_t _column, _row;
};

#endif //ARKOI_LANGUAGE_SCANNER_H
