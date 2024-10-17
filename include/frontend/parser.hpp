#pragma once

#include <functional>
#include <vector>
#include <stack>

#include "token.hpp"
#include "utils/utils.hpp"
#include "frontend/ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> &&tokens) : _tokens(std::move(tokens)) {}

    [[nodiscard]] ProgramNode parse_program();

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    [[nodiscard]] std::unique_ptr<Node> _parse_program_statement();

    void _recover_program();

    [[nodiscard]] std::unique_ptr<FunctionNode> _parse_function(const Token &keyword);

    [[nodiscard]] std::vector<ParameterNode> _parse_parameters();

    void _recover_parameters();

    [[nodiscard]] ParameterNode _parse_parameter();

    [[nodiscard]] Type _parse_type();

    [[nodiscard]] std::unique_ptr<BlockNode> _parse_block();

    [[nodiscard]] std::unique_ptr<Node> _parse_block_statement();

    void _recover_block();

    [[nodiscard]] std::unique_ptr<ReturnNode> _parse_return(const Token &keyword);

    [[nodiscard]] std::unique_ptr<IfNode> _parse_if(const Token &keyword);

    [[nodiscard]] std::unique_ptr<CallNode> _parse_call(const Token &identifier);

    [[nodiscard]] std::unique_ptr<Node> _parse_expression();

    [[nodiscard]] std::unique_ptr<Node> _parse_term();

    [[nodiscard]] std::unique_ptr<Node> _parse_factor();

    [[nodiscard]] std::unique_ptr<Node> _parse_primary();

    [[nodiscard]] std::shared_ptr<SymbolTable> _current_scope();

    std::shared_ptr<SymbolTable> _enter_scope();

    void _exit_scope();

    [[nodiscard]] const Token &_current();

    void _next();

    const Token &_consume_any();

    const Token &_consume(Token::Type type);

    [[nodiscard]] std::optional<Token> _try_consume(const std::function<bool(const Token &)> &predicate);

    std::optional<Token> _try_consume(Token::Type type);

    [[nodiscard]] static BinaryNode::Operator _to_binary_operator(const Token &token);

    [[nodiscard]] static bool _is_factor_operator(const Token &token);

    [[nodiscard]] static bool _is_term_operator(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::vector<Token> _tokens;
    size_t _position{};
    bool _failed{};
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
    UnexpectedToken(const std::string &expected, const Token &got)
        : ParserError("Expected " + expected + " but got " + to_string(got.type())) {}
};