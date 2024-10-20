#pragma once

#include "semantic/symbol_table.hpp"
#include "semantic/type.hpp"
#include "utils/visitor.hpp"
#include "token.hpp"

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(node::Visitor &visitor) = 0;
};

namespace node {

class Program : public Node {
public:
    Program(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; };

    [[nodiscard]] auto &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class Block : public Node {
public:
    Block(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; };

    [[nodiscard]] auto &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class Parameter : public Node {
public:
    Parameter(Token name, Type type) : _type(type), _name(std::move(name)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_symbol(Symbol symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() { return _type; }

private:
    Symbol _symbol{};
    Type _type;
    Token _name;
};

class Function : public Node {
public:
    Function(Token name, std::vector<Parameter> &&parameters, Type type,
             std::unique_ptr<Block> &&block,
             std::shared_ptr<SymbolTable> table)
        : _parameters(std::move(parameters)), _table(std::move(table)), _block(std::move(block)),
          _name(std::move(name)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &parameters() { return _parameters; }

    void set_symbol(Symbol symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &table() const { return _table; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &block() { return _block; }

private:
    std::vector<Parameter> _parameters;
    std::shared_ptr<SymbolTable> _table;
    Symbol _symbol{};
    std::unique_ptr<Block> _block;
    Token _name;
    Type _type;
};

class Return : public Node {
public:
    Return(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::unique_ptr<Node> &&node) { _expression = std::move(node); }

    [[nodiscard]] auto &expression() const { return _expression; }

    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::unique_ptr<Node> _expression;
    std::optional<Type> _type{};
};

class If : public Node {
public:
    using Else = std::variant<std::unique_ptr<Block>, std::unique_ptr<If>, std::unique_ptr<Node>>;
    using Then = std::variant<std::unique_ptr<Block>, std::unique_ptr<Node>>;

public:
    If(std::unique_ptr<Node> &&condition, Then &&then, std::optional<Else> &&branch)
        : _condition(std::move(condition)), _branch(std::move(branch)), _then(std::move(then)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_condition(std::unique_ptr<Node> &&condition) { _condition = std::move(condition); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &branch() const { return _branch; }

    [[nodiscard]] auto &then() const { return _then; }

private:
    std::unique_ptr<Node> _condition;
    std::optional<Else> _branch;
    Then _then;
};

class Call : public Node {
public:
    Call(Token name, std::vector<std::unique_ptr<Node>> &&arguments)
        : _arguments(std::move(arguments)), _name(std::move(name)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_symbol(Symbol symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &arguments() { return _arguments; }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::vector<std::unique_ptr<Node>> _arguments;
    Symbol _symbol{};
    Token _name;
};

class Integer : public Node {
public:
    Integer(Token value) : _value(std::move(value)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Token _value;
};

class Floating : public Node {
public:
    Floating(Token value) : _value(std::move(value)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Token _value;
};

class Boolean : public Node {
public:
    Boolean(bool value) : _value(value) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    bool _value;
};

class Identifier : public Node {
public:
    Identifier(Token value) : _value(std::move(value)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_symbol(Symbol symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Symbol _symbol{};
    Token _value;
};

class Binary : public Node {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    Binary(std::unique_ptr<Node> &&left, Operator op, std::unique_ptr<Node> &&right)
        : _left(std::move(left)), _right(std::move(right)), _op(op) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &op() const { return _op; }

    void set_right(std::unique_ptr<Node> &&node) { _right = std::move(node); }

    [[nodiscard]] auto &right() const { return _right; }

    void set_left(std::unique_ptr<Node> &&node) { _left = std::move(node); }

    [[nodiscard]] auto &left() const { return _left; }

    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::unique_ptr<Node> _left, _right;
    std::optional<Type> _type{};
    Operator _op;
};

class Cast : public Node {
public:
    Cast(std::unique_ptr<Node> &&expression, Type from, Type to)
        : _expression(std::move(expression)), _from(from), _to(to) {}

    Cast(std::unique_ptr<Node> &&expression, Type to)
        : _expression(std::move(expression)), _from(), _to(to) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; }

    void set_from(Type type) { _from = type; }

    [[nodiscard]] auto &from() { return _from; }

    [[nodiscard]] auto &to() { return _to; }

private:
    std::unique_ptr<Node> _expression;
    std::optional<Type> _from;
    Type _to;
};

}