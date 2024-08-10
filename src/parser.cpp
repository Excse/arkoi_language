//
// Created by timo on 8/6/24.
//

#include "parser.h"

Program Parser::parse_program() {
    std::vector<std::unique_ptr<Node>> statements;

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
        } catch (const UnexpectedEndOfTokens &error) {
            std::cout << error.what() << std::endl;
            break;
        }
    }

    return Program(std::move(statements));
}

std::unique_ptr<Node> Parser::_parse_program_statement() {
    auto &current = _current();
    switch (current.type()) {
        case Token::Type::Fun:
            return _parse_function();
        default:
            throw UnexpectedToken("fun", current);
    }
}

void Parser::_recover_program() {
    while (true) {
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::Fun:
            case Token::Type::EndOfFile:
                return;
            default:
                continue;
        }
    }
}

std::unique_ptr<Function> Parser::_parse_function() {
    _consume(Token::Type::Fun);

    auto &name = _consume(Token::Type::Identifier);

    auto arguments = _parse_arguments();

    auto return_type = _parse_type();

    auto block = _parse_block();

    return std::make_unique<Function>(name, std::move(return_type), std::move(block));
}

std::vector<Argument> Parser::_parse_arguments() {
    std::vector<Argument> arguments;

    _consume(Token::Type::LParent);

    while (true) {
        auto &current = _current();
        if (current.type() == Token::Type::EndOfFile) {
            throw UnexpectedEndOfTokens();
        } else if (current.type() == Token::Type::RParent) {
            break;
        }

        if (!arguments.empty()) {
            _consume(Token::Type::Comma);
        }

        try {
            arguments.push_back(_parse_argument());
        } catch (const UnexpectedToken &error) {
            std::cout << error.what() << std::endl;
            _recover_arguments();
        } catch (const UnexpectedEndOfTokens &error) {
            std::cout << error.what() << std::endl;
            break;
        }
    }

    _consume(Token::Type::RParent);

    return arguments;
}

void Parser::_recover_arguments() {
    while (true) {
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::Comma:
            case Token::Type::RParent:
            case Token::Type::EndOfFile:
                return;
            default:
                continue;
        }
    }
}

Argument Parser::_parse_argument() {
    auto &name = _consume(Token::Type::Identifier);

    auto type = _parse_type();

    return {name, std::move(type)};
}

Type Parser::_parse_type() {
    _consume(Token::Type::At);

    auto is_type = [](const Token &token) {
        switch (token.type()) {
            case Token::Type::Bool:
            case Token::Type::U8:
            case Token::Type::S8:
            case Token::Type::U16:
            case Token::Type::S16:
            case Token::Type::U32:
            case Token::Type::S32:
            case Token::Type::U64:
            case Token::Type::S64:
            case Token::Type::USize:
            case Token::Type::SSize:
                return true;
            default:
                return false;
        }
    };

    auto &type = _consume(is_type, "bool, u8, s8, u16, s16, u32, s32, u64, s64, usize, ssize");
    return Type(type);
}

Block Parser::_parse_block() {
    std::vector<std::unique_ptr<Node>> statements;

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
        } catch (const UnexpectedEndOfTokens &error) {
            throw error;
        }
    }

    _consume(Token::Type::RCBracket);

    return Block(std::move(statements));
}

std::unique_ptr<Node> Parser::_parse_block_statement() {
    auto &current = _current();
    switch (current.type()) {
        case Token::Type::Return:
            return _parse_return();
        default:
            throw UnexpectedToken("return", current);
    }
}

void Parser::_recover_block() {
    while (true) {
        _next();

        auto &current = _current();
        switch (current.type()) {
            case Token::Type::RCBracket:
            case Token::Type::Return:
            case Token::Type::EndOfFile:
                return;
            default:
                continue;
        }
    }
}

std::unique_ptr<Return> Parser::_parse_return() {
    _consume(Token::Type::Return);

    auto expression = _parse_expression();

    _consume(Token::Type::Semicolon);

    return std::make_unique<Return>(std::move(expression));
}

std::unique_ptr<Node> Parser::_parse_expression() {
    return _parse_primary();
}

std::unique_ptr<Node> Parser::_parse_primary() {
    if (auto *number = _try_consume(Token::Type::Number)) {
        return std::make_unique<Number>(*number);
    } else if (auto *identifier = _try_consume(Token::Type::Identifier)) {
        return std::make_unique<Identifier>(*identifier);
    }

    auto &current = _current();
    throw UnexpectedToken("number or identifier", current);
}

const Token &Parser::_current() {
    return _tokens[_position];
}

void Parser::_next() {
    _position++;
}

const Token &Parser::_consume(Token::Type type) {
    return _consume([&](const Token &input) { return input.type() == type; }, Token::type_name(type));
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
