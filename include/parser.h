//
// Created by timo on 8/6/24.
//

#ifndef ARKOI_LANGUAGE_PARSER_H
#define ARKOI_LANGUAGE_PARSER_H

#include <functional>
#include <vector>

#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(std::vector<Token> &tokens) : _tokens(std::move(tokens)), _position(0) {}

    [[nodiscard]] Program parse_program();

private:
    [[nodiscard]] std::unique_ptr<Node> _parse_program_statement();

    void _recover_program();

    [[nodiscard]] std::unique_ptr<Function> _parse_function();

    [[nodiscard]] std::vector<Argument> _parse_arguments();

    void _recover_arguments();

    [[nodiscard]] Argument _parse_argument();

    [[nodiscard]] Type _parse_type();

    [[nodiscard]] Block _parse_block();

    [[nodiscard]] std::unique_ptr<Node> _parse_block_statement();

    void _recover_block();

    [[nodiscard]] std::unique_ptr<Return> _parse_return();

    [[nodiscard]] std::unique_ptr<Node> _parse_expression();

    [[nodiscard]] std::unique_ptr<Node> _parse_primary();

    [[nodiscard]] const Token &_current_token();

    void _next_token();

    const Token &_consume(Token::Type type);

    const Token &_consume(const std::function<bool(const Token &)> &predicate, const std::string &expected);

    [[nodiscard]] const Token *_try_consume(Token::Type type);

    [[nodiscard]] const Token *_try_consume(const std::function<bool(const Token &)> &predicate);

private:
    std::vector<Token> _tokens;
    size_t _position;
};

class ParserError : public std::runtime_error {
public:
    explicit ParserError(const std::string &error) : std::runtime_error(error) {}
};

class UnexpectedEndOfTokens : public ParserError {
public:
    UnexpectedEndOfTokens() : ParserError("Unexpectedly reached the End Of Tokens") {}
};

class UnexpectedToken : public ParserError {
public:
    explicit UnexpectedToken(const std::string &expected, const Token &got)
            : ParserError("Expected " + expected + " but got " + Token::type_name(got.type())) {}
};

#endif //ARKOI_LANGUAGE_PARSER_H
