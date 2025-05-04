#pragma once

#include <functional>
#include <stack>
#include <vector>

#include "ast/nodes.hpp"
#include "front/token.hpp"
#include "utils/utils.hpp"

namespace arkoi::front {

class Parser {
public:
    explicit Parser(std::vector<Token> &&tokens) : _tokens(std::move(tokens)) {}

    [[nodiscard]] ast::Program parse_program();

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_program_statement();

    void _recover_program();

    [[nodiscard]] std::unique_ptr<ast::Function> _parse_function(const Token &keyword);

    [[nodiscard]] std::vector<ast::Parameter> _parse_parameters();

    void _recover_parameters();

    [[nodiscard]] ast::Parameter _parse_parameter();

    [[nodiscard]] sem::Type _parse_type();

    [[nodiscard]] std::unique_ptr<ast::Block> _parse_block();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_block_statement();

    void _recover_block();

    [[nodiscard]] std::unique_ptr<ast::Return> _parse_return(const Token &keyword);

    [[nodiscard]] std::unique_ptr<ast::If> _parse_if(const Token &keyword);

    [[nodiscard]] std::unique_ptr<ast::Assign> _parse_assign(const Token &name);

    [[nodiscard]] std::unique_ptr<ast::Variable> _parse_variable(const Token &name);

    [[nodiscard]] std::unique_ptr<ast::Call> _parse_call(const Token &name);

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_expression();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_comparison();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_term();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_factor();

    [[nodiscard]] std::unique_ptr<ast::Node> _parse_primary();

    [[nodiscard]] std::shared_ptr<sem::SymbolTable> _current_scope();

    std::shared_ptr<sem::SymbolTable> _enter_scope();

    void _exit_scope();

    [[nodiscard]] const Token &_current();

    void _next();

    const Token &_consume_any();

    const Token &_consume(Token::Type type);

    [[nodiscard]] std::optional<Token> _try_consume(const std::function<bool(const Token &)> &predicate);

    std::optional<Token> _try_consume(Token::Type type);

    [[nodiscard]] static ast::Binary::Operator _to_binary_operator(const Token &token);

    [[nodiscard]] static bool _is_factor_operator(const Token &token);

    [[nodiscard]] static bool _is_comparison_operator(const Token &token);

    [[nodiscard]] static bool _is_term_operator(const Token &token);

private:
    std::stack<std::shared_ptr<sem::SymbolTable>> _scopes{};
    std::vector<Token> _tokens;
    size_t _position{};
    bool _failed{};
};

class ParserError : public std::runtime_error {
public:
    explicit ParserError(const std::string &error) : std::runtime_error(error) {}
};

class UnexpectedEndOfTokens final : public ParserError {
public:
    UnexpectedEndOfTokens() : ParserError("Unexpectedly reached the End Of Tokens") {}
};

class UnexpectedToken final : public ParserError {
public:
    UnexpectedToken(const std::string &expected, const Token &got)
        : ParserError("Expected " + expected + " but got " + to_string(got.type())) {}
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
