#ifndef ARKOI_LANGUAGE_AST_H
#define ARKOI_LANGUAGE_AST_H

#include <utility>
#include <vector>
#include <memory>

#include "visitor.h"
#include "token.h"

class SymbolTable;

class Symbol;

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

class TypeNode : public Node {
public:
    static const TypeNode TYPE_U8;
    static const TypeNode TYPE_S8;
    static const TypeNode TYPE_U16;
    static const TypeNode TYPE_S16;
    static const TypeNode TYPE_U32;
    static const TypeNode TYPE_S32;
    static const TypeNode TYPE_U64;
    static const TypeNode TYPE_S64;
    static const TypeNode TYPE_USIZE;
    static const TypeNode TYPE_SSIZE;

public:
    explicit TypeNode(Token token) : _token(token) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &token() const { return _token; }

private:
    Token _token;
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
    ParameterNode(Token name, TypeNode type) : _type(std::move(type)), _name(name) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() { return _type; }

private:
    std::shared_ptr<Symbol> _symbol{};
    TypeNode _type;
    Token _name;
};

class FunctionNode : public Node {
public:
    FunctionNode(Token name, std::vector<ParameterNode> &&parameters, TypeNode return_type, BlockNode &&block,
                 std::shared_ptr<SymbolTable> table)
            : _parameters(std::move(parameters)), _table(std::move(table)), _return_type(std::move(return_type)),
            _block(std::move(block)), _name(name) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &return_type() { return _return_type; }

    [[nodiscard]] auto &parameters() { return _parameters; }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &table() const { return _table; }

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &block() { return _block; }

private:
    std::vector<ParameterNode> _parameters;
    std::shared_ptr<SymbolTable> _table;
    std::shared_ptr<Symbol> _symbol{};
    TypeNode _return_type;
    BlockNode _block;
    Token _name;
};

class ReturnNode : public Node {
public:
    explicit ReturnNode(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::unique_ptr<Node> &&node) { _expression = std::move(node); }

    [[nodiscard]] auto &expression() const { return _expression; }

private:
    std::unique_ptr<Node> _expression;
};

class NumberNode : public Node {
public:
    explicit NumberNode(Token value) : _value(value) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() const { return _value; }

private:
    Token _value;
};

class IdentifierNode : public Node {
public:
    explicit IdentifierNode(Token value) : _value(value) {}

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
    enum class Type {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryNode(std::unique_ptr<Node> &&left, Type type, std::unique_ptr<Node> &&right)
            : _left(std::move(left)), _right(std::move(right)), _type(type) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; }

    void set_right(std::unique_ptr<Node> &&node) { _right = std::move(node); }

    [[nodiscard]] auto &right() const { return _right; }

    void set_left(std::unique_ptr<Node> &&node) { _left = std::move(node); }

    [[nodiscard]] auto &left() const { return _left; }

private:
    std::unique_ptr<Node> _left, _right;
    Type _type;
};

class CastNode : public Node {
public:
    CastNode(std::unique_ptr<Node> &&expression, TypeNode to)
            : _expression(std::move(expression)), _to(std::move(to)) {}

    void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; }

    [[nodiscard]] auto &to() { return _to; }

private:
    std::unique_ptr<Node> _expression;
    TypeNode _to;
};

#endif //ARKOI_LANGUAGE_AST_H
