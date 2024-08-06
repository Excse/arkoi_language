#ifndef SPL_LEXER_H
#define SPL_LEXER_H

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
    explicit UnexpectedChar(const std::string &error) : std::runtime_error(error) {}
};

class Scanner {
public:
    explicit Scanner(std::string_view data) : _data(data), _position(0) {}

    std::vector<Token> tokenize();

private:
    Token _next_token();

    Token _lex_comment();

    Token _lex_identifier();

    Token _lex_number();

    Token _lex_special();

    void _consume(char expected);

    void _consume(const std::function<bool(char)> &predicate, const std::string &error);

    bool _try_consume(char expected);

    bool _try_consume(const std::function<bool(char)> &predicate);

    [[nodiscard]] char _current() { return _data[_position]; }

    static bool _is_digit(char input);

    static bool _is_ident_start(char input);

    static bool _is_ident(char input);

    static bool _is_not_newline(char input);

    static bool _is_space(char input);

    static bool _is_hex(char input);

private:
    std::string_view _data;
    size_t _position;
};

#endif //SPL_LEXER_H
