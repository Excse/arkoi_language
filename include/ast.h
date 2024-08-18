#ifndef ARKOI_LANGUAGE_AST_H
#define ARKOI_LANGUAGE_AST_H

#include <utility>
#include <vector>
#include <memory>

#include "visitor.h"

class SymbolTable;

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(Visitor &visitor) const = 0;
};

class Program : public Node {
public:
    Program(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
            : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class Type : public Node {
public:
    explicit Type(Token token) : _token(token) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &token() const { return _token; }

private:
    Token _token;
};

class Block : public Node {
public:
    Block(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
            : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class Parameter : public Node {
public:
    Parameter(Token name, Type type) : _name(name), _type(std::move(type)) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &name() const { return _name; }

    [[nodiscard]] const Type &type() const { return _type; }

private:
    Token _name;
    Type _type;
};

class Function : public Node {
public:
    Function(Token name, std::vector<Parameter> &&parameters, Type return_type, Block &&block,
             std::shared_ptr<SymbolTable> table)
            : _table(std::move(table)), _parameters(std::move(parameters)), _return_type(std::move(return_type)),
              _block(std::move(block)), _name(name) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<Parameter> &parameters() const { return _parameters; }

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; }

    [[nodiscard]] const Type &return_type() const { return _return_type; }

    [[nodiscard]] const Block &block() const { return _block; }

    [[nodiscard]] const Token &name() const { return _name; }

private:
    std::shared_ptr<SymbolTable> _table;
    std::vector<Parameter> _parameters;
    Type _return_type;
    Block _block;
    Token _name;
};

class Return : public Node {
public:
    explicit Return(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Node &expression() const { return *_expression; }

private:
    std::unique_ptr<Node> _expression;
};

class Number : public Node {
public:
    explicit Number(Token value) : _value(value) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &value() const { return _value; }

private:
    Token _value;
};

class Identifier : public Node {
public:
    explicit Identifier(Token value) : _value(value) {}

    void accept(Visitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &value() const { return _value; }

private:
    Token _value;
};

#endif //ARKOI_LANGUAGE_AST_H
