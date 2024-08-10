//
// Created by timo on 8/6/24.
//

#ifndef ARKOI_LANGUAGE_AST_H
#define ARKOI_LANGUAGE_AST_H

#include <utility>
#include <vector>
#include <memory>

#include "visitor.h"

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(Visitor &visitor) = 0;
};

class Program : public Node {
public:
    explicit Program(std::vector<std::unique_ptr<Node>> &statements) : _statements(std::move(statements)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
};

class Type : public Node {
public:
    explicit Type(Token token) : _token(token) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] const Token &token() { return _token; }

private:
    Token _token;
};

class Block : public Node {
public:
    explicit Block(std::vector<std::unique_ptr<Node>> &statements) : _statements(std::move(statements)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
};

class Argument : public Node {
public:
    Argument(Token name, Type type) : _name(name), _type(std::move(type)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] const Token &name() const { return _name; }

    [[nodiscard]] const Type &type() const { return _type; }

private:
    Token _name;
    Type _type;
};

class Function : public Node {
public:
    Function(Token name, Type return_type, Block block)
            : _return_type(std::move(return_type)), _block(std::move(block)), _name(name) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] const Type &return_type() const { return _return_type; }

    [[nodiscard]] const Block &block() const { return _block; }

    [[nodiscard]] const Token &name() const { return _name; }

private:
    Type _return_type;
    Block _block;
    Token _name;
};

#endif //ARKOI_LANGUAGE_AST_H
