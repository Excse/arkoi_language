#ifndef ARKOI_LANGUAGE_AST_H
#define ARKOI_LANGUAGE_AST_H

#include <utility>
#include <vector>
#include <memory>

#include "visitor.h"
#include "token.h"

class SymbolTable;

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(NodeVisitor &visitor) const = 0;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
            : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class TypeNode : public Node {
public:
    explicit TypeNode(Token token) : _token(token) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &token() const { return _token; }

private:
    Token _token;
};

class BlockNode : public Node {
public:
    BlockNode(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<SymbolTable> table)
            : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>> &statements() const { return _statements; };

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; };

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<SymbolTable> _table;
};

class ParameterNode : public Node {
public:
    ParameterNode(Token name, TypeNode type) : _name(name), _type(std::move(type)) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &name() const { return _name; }

    [[nodiscard]] const TypeNode &type() const { return _type; }

private:
    Token _name;
    TypeNode _type;
};

class FunctionNode : public Node {
public:
    FunctionNode(Token name, std::vector<ParameterNode> &&parameters, TypeNode return_type, BlockNode &&block,
                 std::shared_ptr<SymbolTable> table)
            : _table(std::move(table)), _parameters(std::move(parameters)), _return_type(std::move(return_type)),
              _block(std::move(block)), _name(name) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::vector<ParameterNode> &parameters() const { return _parameters; }

    [[nodiscard]] const std::shared_ptr<SymbolTable> &table() const { return _table; }

    [[nodiscard]] const TypeNode &return_type() const { return _return_type; }

    [[nodiscard]] const BlockNode &block() const { return _block; }

    [[nodiscard]] const Token &name() const { return _name; }

private:
    std::shared_ptr<SymbolTable> _table;
    std::vector<ParameterNode> _parameters;
    TypeNode _return_type;
    BlockNode _block;
    Token _name;
};

class ReturnNode : public Node {
public:
    explicit ReturnNode(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Node &expression() const { return *_expression; }

private:
    std::unique_ptr<Node> _expression;
};

class NumberNode : public Node {
public:
    explicit NumberNode(Token value) : _value(value) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &value() const { return _value; }

private:
    Token _value;
};

class IdentifierNode : public Node {
public:
    explicit IdentifierNode(Token value) : _value(value) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Token &value() const { return _value; }

private:
    Token _value;
};

class BinaryNode : public Node {
public:
    enum class Type {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryNode(std::unique_ptr<Node> &&left, Type type, std::unique_ptr<Node> &&right)
            : _left(std::move(left)), _type(type), _right(std::move(right)) {}

    void accept(NodeVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Node &right() const { return *_right; }

    [[nodiscard]] const Node &left() const { return *_left; }

    [[nodiscard]] const Type &type() const { return _type; }

private:
    std::unique_ptr<Node> _left, _right;
    Type _type;
};

#endif //ARKOI_LANGUAGE_AST_H
