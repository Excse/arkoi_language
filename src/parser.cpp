//
// Created by timo on 8/6/24.
//

#include "parser.h"

Program Parser::parse_program() {
    std::vector<std::unique_ptr<Node>> statements;

    while (true) {
        auto current = _current_token();
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

    return Program(statements);
}

std::unique_ptr<Node> Parser::_parse_program_statement() {
    auto current = _current_token();

    if (current.type() == Token::Type::Fun) {
        return _parse_function();
    }

    throw UnexpectedToken("fun", current);
}

void Parser::_recover_program() {
    _next_token();

    while (true) {
        auto current = _current_token();
        switch (current.type()) {
            case Token::Type::Fun:
            case Token::Type::EndOfFile:
                return;
            default:
                _next_token();
                break;
        }
    }
}

std::unique_ptr<Function> Parser::_parse_function() {
    _consume(Token::Type::Fun);

    const Token &name = _consume(Token::Type::Identifier);

    std::vector<Argument> arguments = _parse_arguments();

    Type return_type = _parse_type();

    Block block = _parse_block();

    return std::make_unique<Function>(name, return_type, std::move(block));
}

std::vector<Argument> Parser::_parse_arguments() {
    std::vector<Argument> arguments;

    _consume(Token::Type::LParent);

    while (true) {
        auto current = _current_token();
        if (current.type() == Token::Type::EndOfFile) {
            throw UnexpectedEndOfTokens();
        } else if (current.type() == Token::Type::RParent) {
            break;
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
    _next_token();

    while (true) {
        auto current = _current_token();
        switch (current.type()) {
            case Token::Type::Identifier:
            case Token::Type::RParent:
            case Token::Type::EndOfFile:
                return;
            default:
                _next_token();
                break;
        }
    }
}

Argument Parser::_parse_argument() {
    const Token &name = _consume(Token::Type::Identifier);

    Type type = _parse_type();

    return {name, type};
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

    const Token &token = _consume(is_type, "bool, u8, s8, u16, s16, u32, s32, u64, s64, usize, ssize");

    return Type(token);
}

Block Parser::_parse_block() {
    std::vector<std::unique_ptr<Node>> statements;

    _consume(Token::Type::LCBracket);

    while (true) {
        auto current = _current_token();
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

    return Block(statements);
}

void Parser::_recover_block() {
    _next_token();

    while (true) {
        auto current = _current_token();
        switch (current.type()) {
            case Token::Type::RCBracket:
            case Token::Type::EndOfFile:
                return;
            default:
                _next_token();
                break;
        }
    }
}

std::unique_ptr<Node> Parser::_parse_block_statement() {
    auto current = _current_token();

    throw UnexpectedToken("nothing", current);
}

const Token &Parser::_current_token() {
    return _tokens[_position];
}

void Parser::_next_token() {
    _position++;
}

const Token &Parser::_consume(Token::Type type) {
    return _consume([&](const Token &input) { return input.type() == type; }, Token::type_name(type));
}

bool Parser::_try_consume(Token::Type type) {
    try {
        _consume(type);
        return true;
    } catch (...) {
        return false;
    }
}

const Token &Parser::_consume(const std::function<bool(const Token &)> &predicate, const std::string &expected) {
    const Token &current = _current_token();
    if (_position >= _tokens.size()) {
        throw UnexpectedEndOfTokens();
    }

    if (!predicate(current)) {
        throw UnexpectedToken(expected, current);
    }

    _next_token();

    return current;
}

const Token *Parser::_try_consume(const std::function<bool(const Token &)> &predicate) {
    try {
        const Token &consumed = _consume(predicate, "");
        return &consumed;
    } catch (...) {
        return nullptr;
    }
}
