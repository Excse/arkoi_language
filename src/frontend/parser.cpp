#include "frontend/parser.hpp"

#include "semantic/symbol_table.hpp"

node::Program Parser::parse_program() {
    std::vector<std::unique_ptr<Node>> statements;
    auto own_scope = _enter_scope();

    while (true) {
        const auto &current = _current();
        if (current.type() == Token::Type::Comment || current.type() == Token::Type::Newline) {
            _next();
            continue;
        } else if (current.type() == Token::Type::EndOfFile) {
            break;
        }

        try {
            statements.push_back(_parse_program_statement());
        } catch (const UnexpectedToken &error) {
            std::cout << error.what() << std::endl;
            _recover_program();
            _failed = true;
        } catch (const UnexpectedEndOfTokens &error) {
            std::cout << error.what() << std::endl;
            _failed = true;
            break;
        }
    }

    _exit_scope();

    return {std::move(statements), own_scope};
}

std::unique_ptr<Node> Parser::_parse_program_statement() {
    const auto &current = _consume_any();
    if (current.type() == Token::Type::Fun) {
        return _parse_function(current);
    } else {
        throw UnexpectedToken("fun", current);
    }
}

void Parser::_recover_program() {
    while (true) {
        const auto &current = _current();
        switch (current.type()) {
            case Token::Type::Fun:
            case Token::Type::EndOfFile: return;
            default: _next();
        }
    }
}

std::unique_ptr<node::Function> Parser::_parse_function(const Token &) {
    auto own_scope = _enter_scope();

    const auto &name = _consume(Token::Type::Identifier);

    auto parameters = _parse_parameters();

    auto return_type = _parse_type();

    _consume(Token::Type::Newline);

    auto block = _parse_block();

    _exit_scope();

    return std::make_unique<node::Function>(name, std::move(parameters), return_type, std::move(block), own_scope);
}

std::vector<node::Parameter> Parser::_parse_parameters() {
    std::vector<node::Parameter> parameters;

    _consume(Token::Type::LParent);

    while (true) {
        const auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) throw UnexpectedEndOfTokens();
        if (current.type() == Token::Type::RParent) break;

        if (!parameters.empty()) {
            _consume(Token::Type::Comma);
        }

        try {
            parameters.push_back(_parse_parameter());
        } catch (const UnexpectedToken &error) {
            std::cout << error.what() << std::endl;
            _recover_parameters();
            _failed = true;
        } catch (const UnexpectedEndOfTokens &error) {
            std::cout << error.what() << std::endl;
            _failed = true;
            break;
        }
    }

    _consume(Token::Type::RParent);

    return parameters;
}

void Parser::_recover_parameters() {
    while (true) {
        const auto &current = _current();
        switch (current.type()) {
            case Token::Type::Comma:
            case Token::Type::RParent:
            case Token::Type::EndOfFile: return;
            default: _next();
        }
    }
}

node::Parameter Parser::_parse_parameter() {
    const auto &name = _consume(Token::Type::Identifier);

    auto type = _parse_type();

    return {name, type};
}

Type Parser::_parse_type() {
    _consume(Token::Type::At);

    auto token = _consume_any();
    switch (token.type()) {
        case Token::Type::U8: return type::Integral(Size::BYTE, false);
        case Token::Type::S8: return type::Integral(Size::BYTE, true);
        case Token::Type::U16: return type::Integral(Size::WORD, false);
        case Token::Type::S16: return type::Integral(Size::WORD, true);
        case Token::Type::U32: return type::Integral(Size::DWORD, false);
        case Token::Type::S32: return type::Integral(Size::DWORD, true);
        case Token::Type::U64: return type::Integral(Size::QWORD, false);
        case Token::Type::S64: return type::Integral(Size::QWORD, true);
        case Token::Type::USize: return type::Integral(Size::QWORD, false);
        case Token::Type::SSize: return type::Integral(Size::QWORD, true);
        case Token::Type::F32: return type::Floating(Size::DWORD);
        case Token::Type::F64: return type::Floating(Size::QWORD);
        case Token::Type::Bool: return type::Boolean();
        default: throw UnexpectedToken("bool, u8, s8, u16, s16, u32, s32, u64, s64, usize, ssize, bool", token);
    }
}

std::unique_ptr<node::Block> Parser::_parse_block() {
    std::vector<std::unique_ptr<Node>> statements;

    auto own_scope = _enter_scope();
    _consume(Token::Type::Indentation);

    while (true) {
        const auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) throw UnexpectedEndOfTokens();
        if (current.type() == Token::Type::Dedentation) break;

        try {
            statements.push_back(_parse_block_statement());
        } catch (const UnexpectedToken &error) {
            std::cout << error.what() << std::endl;
            _recover_block();
            _failed = true;
        } catch (const UnexpectedEndOfTokens &error) {
            throw error;
        }
    }

    _consume(Token::Type::Dedentation);
    _exit_scope();

    return std::make_unique<node::Block>(std::move(statements), own_scope);
}

std::unique_ptr<Node> Parser::_parse_block_statement() {
    std::unique_ptr<Node> result;

    const auto &current = _consume_any();
    if (current.type() == Token::Type::Return) {
        result = _parse_return(current);
        _consume(Token::Type::Newline);
    } else if (current.type() == Token::Type::Identifier) {
        result = _parse_call(current);
        _consume(Token::Type::Newline);
    } else if (current.type() == Token::Type::If) {
        result = _parse_if(current);
        // Don't need to consume a newline, as the then node already parsed it
    } else {
        throw UnexpectedToken("return, if or call", current);
    }

    return result;
}

void Parser::_recover_block() {
    while (true) {
        const auto &current = _current();
        switch (current.type()) {
            case Token::Type::Dedentation:
            case Token::Type::Return:
            case Token::Type::EndOfFile: return;
            default: _next();
        }
    }
}

std::unique_ptr<node::Return> Parser::_parse_return(const Token &) {
    auto expression = _parse_expression();

    return std::make_unique<node::Return>(std::move(expression));
}

std::unique_ptr<node::If> Parser::_parse_if(const Token &) {
    auto expression = _parse_expression();

    node::If::ThenType then;
    if (_try_consume(Token::Type::Newline)) {
        then = _parse_block();
    } else {
        then = _parse_block_statement();
    }

    if (!_try_consume(Token::Type::Else)) {
        return std::make_unique<node::If>(std::move(expression), std::move(then), std::nullopt);
    }

    if (auto token = _try_consume(Token::Type::If)) {
        return std::make_unique<node::If>(std::move(expression), std::move(then), _parse_if(*token));
    }

    node::If::ElseType _else;
    if (_try_consume(Token::Type::Newline)) {
        _else = _parse_block();
    } else {
        _else = _parse_block_statement();
    }

    return std::make_unique<node::If>(std::move(expression), std::move(then), std::move(_else));
}

std::unique_ptr<node::Call> Parser::_parse_call(const Token &identifier) {
    _consume(Token::Type::LParent);

    std::vector<std::unique_ptr<Node>> arguments;
    while (true) {
        const auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) throw UnexpectedEndOfTokens();
        if (current.type() == Token::Type::RParent) break;

        if (!arguments.empty()) {
            _consume(Token::Type::Comma);
        }

        arguments.push_back(_parse_expression());
    }

    _consume(Token::Type::RParent);

    return std::make_unique<node::Call>(identifier, std::move(arguments));
}

std::unique_ptr<Node> Parser::_parse_expression() {
    return _parse_term();
}

std::unique_ptr<Node> Parser::_parse_term() {
    auto expression = _parse_factor();

    while (auto op = _try_consume(_is_term_operator)) {
        auto type = _to_binary_operator(op.value());

        expression = std::make_unique<node::Binary>(std::move(expression), type, _parse_factor());
    }

    return expression;
}

std::unique_ptr<Node> Parser::_parse_factor() {
    auto expression = _parse_primary();

    while (auto op = _try_consume(_is_factor_operator)) {
        auto type = _to_binary_operator(op.value());

        expression = std::make_unique<node::Binary>(std::move(expression), type, _parse_primary());
    }

    return expression;
}


std::unique_ptr<Node> Parser::_parse_primary() {
    const auto &consumed = _consume_any();
    if (consumed.type() == Token::Type::Integer) {
        auto node = std::make_unique<node::Integer>(consumed);
        if (_current().type() != Token::Type::At) return node;

        return std::make_unique<node::Cast>(std::move(node), _parse_type());
    } else if (consumed.type() == Token::Type::Floating) {
        auto node = std::make_unique<node::Floating>(consumed);
        if (_current().type() != Token::Type::At) return node;

        return std::make_unique<node::Cast>(std::move(node), _parse_type());
    } else if (consumed.type() == Token::Type::Identifier) {
        auto node = std::make_unique<node::Identifier>(consumed);
        if (_current().type() != Token::Type::LParent) return node;

        return _parse_call(consumed);
    } else if (consumed.type() == Token::Type::True) {
        return std::make_unique<node::Boolean>(true);
    } else if (consumed.type() == Token::Type::False) {
        return std::make_unique<node::Boolean>(false);
    } else if (consumed.type() == Token::Type::LParent) {
        auto expression = _parse_expression();
        _consume(Token::Type::RParent);
        return expression;
    }

    throw UnexpectedToken("integer, float, identifier, function call, grouping, true or false", consumed);
}

std::shared_ptr<SymbolTable> Parser::_current_scope() {
    return _scopes.top();
}

std::shared_ptr<SymbolTable> Parser::_enter_scope() {
    if (_scopes.empty()) {
        return _scopes.emplace(std::make_shared<SymbolTable>());
    }

    auto parent = _current_scope();
    return _scopes.emplace(std::make_shared<SymbolTable>(parent));
}

void Parser::_exit_scope() {
    _scopes.pop();
}

const Token &Parser::_current() {
    return _tokens[_position];
}

void Parser::_next() {
    if (_position >= _tokens.size()) throw UnexpectedEndOfTokens();
    _position++;
}

const Token &Parser::_consume_any() {
    const auto &current = _current();
    _next();
    return current;
}

const Token &Parser::_consume(Token::Type type) {
    const auto &current = _current();
    _next();

    if (current.type() != type) throw UnexpectedToken(to_string(type), current);

    return current;
}

std::optional<Token> Parser::_try_consume(const std::function<bool(const Token &)> &predicate) {
    const auto &current = _current();

    if (!predicate(current)) return std::nullopt;

    _next();

    return current;
}

std::optional<Token> Parser::_try_consume(Token::Type type) {
    const auto &current = _current();

    if (current.type() != type) return std::nullopt;

    _next();

    return current;
}

node::Binary::Operator Parser::_to_binary_operator(const Token &token) {
    switch (token.type()) {
        case Token::Type::Slash: return node::Binary::Operator::Div;
        case Token::Type::Asterisk: return node::Binary::Operator::Mul;
        case Token::Type::Plus: return node::Binary::Operator::Add;
        case Token::Type::Minus: return node::Binary::Operator::Sub;
        default: std::unreachable();
    }
}

bool Parser::_is_factor_operator(const Token &token) {
    return token.type() == Token::Type::Asterisk || token.type() == Token::Type::Slash;
}

bool Parser::_is_term_operator(const Token &token) {
    return token.type() == Token::Type::Plus || token.type() == Token::Type::Minus;
}
