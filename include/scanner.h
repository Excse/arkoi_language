#ifndef ARKOI_LANGUAGE_SCANNER_H
#define ARKOI_LANGUAGE_SCANNER_H

#include <functional>
#include <string>
#include <vector>

#include "token.h"

class Scanner {
private:
    struct Location {
        int64_t column, row;
    };

public:
    explicit Scanner(std::string_view data) : _data(data) {}

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

    [[nodiscard]] bool _is_eof();

    [[nodiscard]] Location _mark_start();

    void _next();

    char _peek();

    void _consume(char expected);

    char _consume(const std::function<bool(char)> &predicate, const std::string &expected);

    [[nodiscard]] bool _try_consume(char expected);

    [[nodiscard]] std::optional<char> _try_consume(const std::function<bool(char)> &predicate);

    [[nodiscard]] static bool _is_digit(char input);

    [[nodiscard]] static bool _is_ident_start(char input);

    [[nodiscard]] static bool _is_ident_inner(char input);

    [[nodiscard]] static bool _is_not_newline(char input);

    [[nodiscard]] static bool _is_ascii(char input);

    [[nodiscard]] static bool _is_space(char input);

    [[nodiscard]] static bool _is_hex_start(char input);

    [[nodiscard]] static bool _is_hex_inner(char input);

private:
    size_t _position{}, _start{};
    int64_t _column{}, _row{};
    std::string_view _data;
    bool _failed{};
};

class ScannerError : public std::runtime_error {
public:
    explicit ScannerError(const std::string &error) : std::runtime_error(error) {}
};

class UnexpectedEndOfFile : public ScannerError {
public:
    UnexpectedEndOfFile() : ScannerError("Unexpectedly reached the End Of File") {}
};

class UnexpectedChar : public ScannerError {
public:
    UnexpectedChar(const std::string &expected, char got)
            : ScannerError("Expected " + expected + " but got " + std::string(1, got)) {}
};

class UnknownChar : public ScannerError {
public:
    explicit UnknownChar(char got) : ScannerError("Didn't expect " + std::string(1, got)) {}
};

class NumberOutOfRange : public ScannerError {
public:
    explicit NumberOutOfRange(const std::string &number) : ScannerError(
            "The number " + number + " exceeds the 64bit limitations.") {}
};

#endif //ARKOI_LANGUAGE_SCANNER_H
