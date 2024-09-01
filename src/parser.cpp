#include "parser.h"

#include "symbol_table.h"

Parser::Parser(std::vector<Token> &&tokens)
        : _scopes(), _tokens(std::move(tokens)), _position(0), _failed(false) {
    auto is_useless = [](const Token &token) { return token.type() == Token::Type::Comment; };
    _tokens.erase(std::remove_if(_tokens.begin(), _tokens.end(), is_useless), _tokens.end());
}

ProgramNode Parser::parse_program() {
    std::vector<std::unique_ptr<Node>> statements;
    auto own_scope = _enter_scope();

    while (true) {
        auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) {
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
    auto &current = _current();
    switch (current.type()) {
        case Token::Type::Fun: return _parse_function();
        default: throw UnexpectedToken("fun", current);
    }
}

void Parser::_recover_program() {
    while (true) {
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::Fun:
            case Token::Type::EndOfFile: return;
            default: continue;
        }
    }
}

std::unique_ptr<FunctionNode> Parser::_parse_function() {
    auto own_scope = _enter_scope();

    _consume(Token::Type::Fun);

    auto &name = _consume(Token::Type::Identifier);

    auto parameters = _parse_parameters();

    auto return_type = _parse_type();

    auto block = _parse_block();

    _exit_scope();

    return std::make_unique<FunctionNode>(name, std::move(parameters), return_type, std::move(block), own_scope);
}

std::vector<ParameterNode> Parser::_parse_parameters() {
    std::vector<ParameterNode> parameters;

    _consume(Token::Type::LParent);

    while (true) {
        auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) {
            throw UnexpectedEndOfTokens();
        } else if (current.type() == Token::Type::RParent) {
            break;
        }

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
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::Comma:
            case Token::Type::RParent:
            case Token::Type::EndOfFile: return;
            default: continue;
        }
    }
}

ParameterNode Parser::_parse_parameter() {
    auto &name = _consume(Token::Type::Identifier);

    auto type = _parse_type();

    return {name, type};
}

TypeNode Parser::_parse_type() {
    _consume(Token::Type::At);

    auto is_type = [](const Token &token) {
        switch (token.type()) {
            case Token::Type::U8:
            case Token::Type::S8:
            case Token::Type::U16:
            case Token::Type::S16:
            case Token::Type::U32:
            case Token::Type::S32:
            case Token::Type::U64:
            case Token::Type::S64:
            case Token::Type::USize:
            case Token::Type::SSize: return true;
            default: return false;
        }
    };

    auto &type = _consume(is_type, "bool, u8, s8, u16, s16, u32, s32, u64, s64, usize, ssize");
    return TypeNode(type);
}

BlockNode Parser::_parse_block() {
    std::vector<std::unique_ptr<Node>> statements;

    auto own_scope = _enter_scope();
    _consume(Token::Type::LCBracket);

    while (true) {
        auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) {
            throw UnexpectedEndOfTokens();
        } else if (current.type() == Token::Type::RCBracket) {
            break;
        }

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

    _consume(Token::Type::RCBracket);
    _exit_scope();

    return {std::move(statements), own_scope};
}

std::unique_ptr<Node> Parser::_parse_block_statement() {
    auto &current = _current();
    switch (current.type()) {
        case Token::Type::Return: return _parse_return();
        default: throw UnexpectedToken("return", current);
    }
}

void Parser::_recover_block() {
    while (true) {
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::RCBracket:
            case Token::Type::Return:
            case Token::Type::EndOfFile: return;
            default: continue;
        }
    }
}

std::unique_ptr<ReturnNode> Parser::_parse_return() {
    _consume(Token::Type::Return);

    auto expression = _parse_expression();

    _consume(Token::Type::Semicolon);

    return std::make_unique<ReturnNode>(std::move(expression));
}

std::unique_ptr<Node> Parser::_parse_expression() {
    return _parse_term();
}

std::unique_ptr<Node> Parser::_parse_term() {
    auto expression = _parse_factor();

    while (auto *operator_token = _try_consume(is_term_operator)) {
        auto type = to_binary_operator(*operator_token);

        expression = std::make_unique<BinaryNode>(std::move(expression), type, _parse_factor());
    }

    return expression;
}

std::unique_ptr<Node> Parser::_parse_factor() {
    auto expression = _parse_primary();

    while (auto operator_token = _try_consume(is_factor_operator)) {
        auto type = to_binary_operator(*operator_token);

        expression = std::make_unique<BinaryNode>(std::move(expression), type, _parse_primary());
    }

    return expression;
}


std::unique_ptr<Node> Parser::_parse_primary() {
    if (auto *number = _try_consume(Token::Type::Number)) {
        return std::make_unique<NumberNode>(*number);
    } else if (auto *identifier = _try_consume(Token::Type::Identifier)) {
        return std::make_unique<IdentifierNode>(*identifier);
    }

    auto &current = _current();
    throw UnexpectedToken("number or identifier", current);
}

std::shared_ptr<SymbolTable> Parser::_current_scope() {
    return _scopes.top();
}

std::shared_ptr<SymbolTable> Parser::_enter_scope() {
    std::shared_ptr<SymbolTable> new_scope;
    if (_scopes.empty()) {
        _scopes.push(std::make_shared<SymbolTable>());
    } else {
        auto parent = _current_scope();
        _scopes.push(std::make_shared<SymbolTable>(parent));
    }

    return _current_scope();
}

std::shared_ptr<SymbolTable> Parser::_exit_scope() {
    auto current = _current_scope();
    _scopes.pop();
    return current;
}

const Token &Parser::_current() {
    return _tokens[_position];
}

void Parser::_next() {
    _position++;
}

const Token &Parser::_consume(Token::Type type) {
    return _consume([&](const Token &input) { return input.type() == type; }, to_string(type));
}

const Token *Parser::_try_consume(Token::Type type) {
    try {
        auto &consumed = _consume(type);
        return &consumed;
    } catch (const ParserError &) {
        return nullptr;
    }
}

const Token &Parser::_consume(const std::function<bool(const Token &)> &predicate, const std::string &expected) {
    auto &current = _current();
    if (_position >= _tokens.size()) {
        throw UnexpectedEndOfTokens();
    }

    if (!predicate(current)) {
        throw UnexpectedToken(expected, current);
    }

    _next();

    return current;
}

const Token *Parser::_try_consume(const std::function<bool(const Token &)> &predicate) {
    try {
        auto &consumed = _consume(predicate, "");
        return &consumed;
    } catch (const ParserError &) {
        return nullptr;
    }
}

BinaryNode::Type Parser::to_binary_operator(const Token &token) {
    switch (token.type()) {
        case Token::Type::Slash:return BinaryNode::Type::Div;
        case Token::Type::Asterisk:return BinaryNode::Type::Mul;
        case Token::Type::Plus:return BinaryNode::Type::Add;
        case Token::Type::Minus:return BinaryNode::Type::Sub;
        default:throw std::invalid_argument("This token is a invalid binary operator.");
    }
}

bool Parser::is_factor_operator(const Token &token) {
    return token.type() == Token::Type::Asterisk || token.type() == Token::Type::Slash;
}

bool Parser::is_term_operator(const Token &token) {
    return token.type() == Token::Type::Plus || token.type() == Token::Type::Minus;
}