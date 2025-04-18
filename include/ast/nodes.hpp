#pragma once

#include <utility>

#include "ast/visitor.hpp"
#include "front/token.hpp"
#include "sem/symbol_table.hpp"
#include "sem/type.hpp"

namespace arkoi::ast {

class Node {
public:
    virtual ~Node() = default;

    virtual void accept(Visitor &visitor) = 0;
};

class Program final : public Node {
public:
    Program(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<sem::SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; }

    [[nodiscard]] auto &table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
};

class Block final : public Node {
public:
    Block(std::vector<std::unique_ptr<Node>> &&statements, std::shared_ptr<sem::SymbolTable> table)
        : _statements(std::move(statements)), _table(std::move(table)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &statements() const { return _statements; }

    [[nodiscard]] auto &table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
};

class Identifier final : public Node {
public:
    enum class Kind {
        Function,
        Variable
    };

public:
    Identifier(front::Token value, Kind kind) : _value(std::move(value)), _kind(kind) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    [[nodiscard]] auto &symbol() const { return _symbol.value(); }

    [[nodiscard]] auto &value() const { return _value; }

    [[nodiscard]] auto &kind() const { return _kind; }

private:
    std::optional<std::shared_ptr<Symbol>> _symbol{};
    front::Token _value;
    Kind _kind;
};

class Parameter final : public Node {
public:
    Parameter(Identifier name, sem::Type type)
        : _name(std::move(name)), _type(std::move(type)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &name() { return _name; }

    [[nodiscard]] auto &type() { return _type; }

private:
    Identifier _name;
    sem::Type _type;
};

class Function final : public Node {
public:
    Function(Identifier name, std::vector<Parameter> &&parameters, sem::Type type,
             std::unique_ptr<Block> &&block, std::shared_ptr<sem::SymbolTable> table)
        : _table(std::move(table)), _parameters(std::move(parameters)), _block(std::move(block)),
          _name(std::move(name)), _type(std::move(type)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &parameters() { return _parameters; }

    [[nodiscard]] auto &table() const { return _table; }

    [[nodiscard]] auto &type() { return _type; }

    [[nodiscard]] auto &block() { return _block; }

    [[nodiscard]] auto &name()  { return _name; }

private:
    std::shared_ptr<sem::SymbolTable> _table;
    std::vector<Parameter> _parameters;
    std::unique_ptr<Block> _block;
    Identifier _name;
    sem::Type _type;
};

class Return final : public Node {
public:
    explicit Return(std::unique_ptr<Node> &&expression) : _expression(std::move(expression)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::unique_ptr<Node> &&node) { _expression = std::move(node); }

    [[nodiscard]] auto &expression() { return _expression; }

    void set_type(sem::Type type) { _type = std::move(type); }

    [[nodiscard]] auto &type() const { return _type.value(); }

private:
    std::unique_ptr<Node> _expression;
    std::optional<sem::Type> _type{};
};

class If final : public Node {
public:
    If(std::unique_ptr<Node> &&condition, std::unique_ptr<Node> &&branch, std::unique_ptr<Node> &&next)
        : _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_condition(std::unique_ptr<Node> &&condition) { _condition = std::move(condition); }

    [[nodiscard]] auto &condition() { return _condition; }

    [[nodiscard]] auto &branch() const { return _branch; }

    [[nodiscard]] auto &next() const { return _next; }

private:
    std::unique_ptr<Node> _next, _branch;
    std::unique_ptr<Node> _condition;
};

class Assign final : public Node {
public:
    Assign(Identifier name, std::unique_ptr<Node> &&expression)
        : _expression(std::move(expression)), _name(std::move(name)){}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::unique_ptr<Node> &&node) { _expression = std::move(node); }

    [[nodiscard]] auto &expression() { return _expression; }

    [[nodiscard]] auto &name() { return _name; }

private:
    std::unique_ptr<Node> _expression;
    Identifier _name;
};

class Call final : public Node {
public:
    Call(Identifier name, std::vector<std::unique_ptr<Node>> &&arguments)
        : _arguments(std::move(arguments)), _name(std::move(name)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &arguments() { return _arguments; }

    [[nodiscard]] auto &name() { return _name; }

private:
    std::vector<std::unique_ptr<Node>> _arguments;
    Identifier _name;
};

class Immediate final : public Node {
public:
    enum class Kind {
        Integer,
        Floating,
        Boolean
    };

public:
    Immediate(front::Token value, Kind kind) : _value(std::move(value)), _kind(kind) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_type(sem::Type type) { _type = std::move(type); }

    [[nodiscard]] auto &type() { return *_type; }

    [[nodiscard]] auto &value() const { return _value; }

    [[nodiscard]] auto &kind() const { return _kind; }

private:
    std::optional<sem::Type> _type;
    front::Token _value;
    Kind _kind;
};

class Binary final : public Node {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
        GreaterThan,
        LessThan,
    };

public:
    Binary(std::unique_ptr<Node> &&left, Operator op, std::unique_ptr<Node> &&right)
        : _left(std::move(left)), _right(std::move(right)), _op(op) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &op() const { return _op; }

    void set_right(std::unique_ptr<Node> &&node) { _right = std::move(node); }

    [[nodiscard]] auto &right() { return _right; }

    void set_left(std::unique_ptr<Node> &&node) { _left = std::move(node); }

    [[nodiscard]] auto &left() { return _left; }

    void set_result_type(sem::Type type) { _result_type = std::move(type); }

    [[nodiscard]] auto &result_type() { return _result_type.value(); }

    void set_op_type(sem::Type type) { _op_type = std::move(type); }

    [[nodiscard]] auto &op_type() const { return _op_type.value(); }

private:
    std::optional<sem::Type> _result_type{}, _op_type{};
    std::unique_ptr<Node> _left, _right;
    Operator _op;
};

class Cast final : public Node {
public:
    Cast(std::unique_ptr<Node> &&expression, sem::Type from, sem::Type to)
        : _expression(std::move(expression)), _from(from), _to(std::move(to)) {}

    Cast(std::unique_ptr<Node> &&expression, sem::Type to)
        : _expression(std::move(expression)), _to(std::move(to)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; }

    void set_from(sem::Type type) { _from = std::move(type); }

    [[nodiscard]] auto &from() const { return _from.value(); }

    [[nodiscard]] auto &to() { return _to; }

private:
    std::unique_ptr<Node> _expression;
    std::optional<sem::Type> _from{};
    sem::Type _to;
};

}  // namespace arkoi::ast

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
