#ifndef ARKOI_LANGUAGE_SCANNER_H
#define ARKOI_LANGUAGE_SCANNER_H

#include <functional>
#include <string>
#include <vector>

#include "cursor.h"
#include "token.h"

class Scanner : Cursor {
public:
    explicit Scanner(std::string_view data) : Cursor(data) {}

    std::vector<Token> tokenize();

private:
    Token _next_token();

    Token _lex_comment();

    Token _lex_identifier();

    Token _lex_number();

    Token _lex_special();

    static bool _is_digit(char input);

    static bool _is_ident_start(char input);

    static bool _is_ident(char input);

    static bool _is_not_newline(char input);

    static bool _is_space(char input);

    static bool _is_hex(char input);
};

#endif //ARKOI_LANGUAGE_SCANNER_H
