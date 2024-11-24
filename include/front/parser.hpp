#pragma once

#include <functional>
#include <vector>
#include <stack>

#include "token.hpp"
#include "def/ast.hpp"
#include "utils/utils.hpp"

namespace arkoi::front {

class Parser {
public:
    Parser(std::vector<Token> &&tokens) : _tokens(std::move(tokens)) {}

    [[nodiscard]] ast::Program parse_program();

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_program_statement();

    void _recover_program();

    [[nodiscard]] std::unique_ptr<ast::Function> _parse_function(const Token &keyword);

    [[nodiscard]] std::vector<ast::Parameter> _parse_parameters();

    void _recover_parameters();

    [[nodiscard]] ast::Parameter _parse_parameter();

    [[nodiscard]] Type _parse_type();

    [[nodiscard]] std::unique_ptr<ast::Block> _parse_block();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_block_statement();

    void _recover_block();

    [[nodiscard]] std::unique_ptr<ast::Return> _parse_return(const Token &keyword);

    [[nodiscard]] std::unique_ptr<ast::If> _parse_if(const Token &keyword);

    [[nodiscard]] std::unique_ptr<ast::Call> _parse_call(const Token &identifier);

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_expression();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_term();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_factor();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_primary();

    [[nodiscard]] std::shared_ptr<mid::SymbolTable> _current_scope();

    std::shared_ptr<mid::SymbolTable> _enter_scope();

    void _exit_scope();

    [[nodiscard]] const Token &_current();

    void _next();

    const Token &_consume_any();

    const Token &_consume(Token::Type type);

    [[nodiscard]] std::optional<Token> _try_consume(const std::function<bool(const Token &)> &predicate);

    std::optional<Token> _try_consume(Token::Type type);

    [[nodiscard]] static ast::Binary::Operator _to_binary_operator(const Token &token);

    [[nodiscard]] static bool _is_factor_operator(const Token &token);

    [[nodiscard]] static bool _is_term_operator(const Token &token);

private:
    std::stack<std::shared_ptr<mid::SymbolTable>> _scopes{};
    std::vector<Token> _tokens;
    size_t _position{};
    bool _failed{};
};

class ParserError : public std::runtime_error {
public:
    ParserError(const std::string &error) : std::runtime_error(error) {}
};

class UnexpectedEndOfTokens : public ParserError {
public:
    UnexpectedEndOfTokens() : ParserError("Unexpectedly reached the End Of Tokens") {}
};

class UnexpectedToken : public ParserError {
public:
    UnexpectedToken(const std::string &expected, const Token &got)
        : ParserError("Expected " + expected + " but got " + to_string(got.type())) {}
};

} // namespace arkoi::front