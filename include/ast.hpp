#pragma once

#include "symbol_table.hpp"
#include "visitor.hpp"
#include "token.hpp"
#include "type.hpp"

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(NodeVisitor &visitor) = 0;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; };

    [[nodiscard]] auto &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class BlockNode : public Node {
public:
    BlockNode(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; };

    [[nodiscard]] auto &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class ParameterNode : public Node {
public:
    ParameterNode(Token name, Type type) : _name(std::move(name)), _type(type) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() { return _type; }

private:
    std::shared_ptr<Symbol> _symbol{};
    Token _name;
    Type _type;
};

class FunctionNode : public Node {
public:
    FunctionNode(Token name, std::vector<ParameterNode> &&parameters, Type type, std::unique_ptr<BlockNode> &&block,
                 std::shared_ptr<SymbolTable> table)
        : _parameters(std::move(parameters)), _table(std::move(table)), _block(std::move(block)),
          _name(std::move(name)), _type(type) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &parameters() { return _parameters; }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &table() const { return _table; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &block() { return _block; }

private:
    std::vector<ParameterNode> _parameters;
    std::shared_ptr<SymbolTable> _table;
    std::shared_ptr<Symbol> _symbol{};
    std::unique_ptr<BlockNode> _block;
    Token _name;
    Type _type;
};

class ReturnNode : public Node {
public:
    explicit ReturnNode(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::unique_ptr<Node> &&node) { _expression = std::move(node); }

    [[nodiscard]] auto &expression() const { return _expression; }

    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::unique_ptr<Node> _expression;
    std::optional<Type> _type{};
};

class IfNode : public Node {
public:
    using Else = std::variant<std::unique_ptr<BlockNode>, std::unique_ptr<IfNode>, std::unique_ptr<Node>>;
    using Then = std::variant<std::unique_ptr<BlockNode>, std::unique_ptr<Node>>;

public:
    explicit IfNode(std::unique_ptr<Node> &&condition, Then &&then, std::optional<Else> &&_else)
        : _condition(std::move(condition)), _else(std::move(_else)), _then(std::move(then)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_condition(std::unique_ptr<Node> &&condition) { _condition = std::move(condition); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &then() const { return _then; }

    [[nodiscard]] auto &els() const { return _else; }

private:
    std::unique_ptr<Node> _condition;
    std::optional<Else> _else;
    Then _then;
};

class CallNode : public Node {
public:
    explicit CallNode(Token name, std::vector<std::unique_ptr<Node>> &&arguments)
        : _arguments(std::move(arguments)), _name(std::move(name)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &arguments() { return _arguments; }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::vector<std::unique_ptr<Node>> _arguments;
    std::shared_ptr<Symbol> _symbol{};
    Token _name;
};

class IntegerNode : public Node {
public:
    explicit IntegerNode(Token value) : _value(std::move(value)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Token _value;
};

class FloatingNode : public Node {
public:
    explicit FloatingNode(Token value) : _value(std::move(value)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Token _value;
};

class BooleanNode : public Node {
public:
    explicit BooleanNode(bool value) : _value(value) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    bool _value;
};

class IdentifierNode : public Node {
public:
    explicit IdentifierNode(Token value) : _value(std::move(value)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &value() const { return _value; }

private:
    std::shared_ptr<Symbol> _symbol{};
    Token _value;
};

class BinaryNode : public Node {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryNode(std::unique_ptr<Node> &&left, Operator op, std::unique_ptr<Node> &&right)
        : _left(std::move(left)), _right(std::move(right)), _op(op) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

class CastNode : public Node {
public:
    CastNode(std::unique_ptr<Node> &&expression, Type from, Type to)
        : _expression(std::move(expression)), _from(from), _to(to) {}

    CastNode(std::unique_ptr<Node> &&expression, Type to)
        : _expression(std::move(expression)), _from(), _to(to) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; }

    void set_from(Type type) { _from = type; }

    [[nodiscard]] auto &from() { return _from; }

    [[nodiscard]] auto &to() { return _to; }

private:
    std::unique_ptr<Node> _expression;
    std::optional<Type> _from;
    Type _to;
};
