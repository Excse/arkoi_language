#pragma once

#include <optional>
#include <utility>
#include <memory>
#include <string>

#include "semantic/symbol_table.hpp"
#include "utils/visitor.hpp"
#include "frontend/ast.hpp"
#include "il/operand.hpp"

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(il::Visitor &visitor) = 0;
};

namespace il {

class Label : public Instruction {
public:
    explicit Label(Symbol symbol) : _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    Symbol _symbol;
};

class Goto : public Instruction {
public:
    explicit Goto(Symbol label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Symbol _label;
};

class IfNot : public Instruction {
public:
    IfNot(Operand condition, Symbol label)
        : _condition(std::move(condition)), _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Operand _condition;
    Symbol _label;
};

class Call : public Instruction {
public:
    explicit Call(Operand result, Symbol symbol)
        : _result(std::move(result)), _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    Operand _result;
    Symbol _symbol;
};

class Return : public Instruction {
public:
    explicit Return(Operand value, Type type) : _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &value() const { return _value; };

private:
    Operand _value;
    Type _type;
};

class Binary : public Instruction {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    Binary(Operand result, Operand left, Operator op, Operand right, Type type)
        : _result(std::move(result)), _left(std::move(left)), _right(std::move(right)), _op(op),
          _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &right() const { return _right; };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(node::Binary::Operator op);

private:
    Operand _result, _left, _right;
    Operator _op;
    Type _type;
};

class Begin : public Instruction {
public:
    explicit Begin(Symbol label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Symbol _label;
};

class End : public Instruction {
public:
    void accept(Visitor &visitor) override { visitor.visit(*this); }
};

class Cast : public Instruction {
public:
    Cast(Operand result, Operand expression, Type from, Type to)
        : _result(std::move(result)), _expression(std::move(expression)), _from(from), _to(to) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    Operand _result, _expression;
    Type _from, _to;
};

class Argument : public Instruction {
public:
    explicit Argument(Operand expression, Symbol symbol)
        : _expression(std::move(expression)), _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::optional<Operand> _result{};
    Operand _expression;
    Symbol _symbol;
};

class Store : public Instruction {
public:
    explicit Store(Operand result, Operand value, Type type)
        : _value(std::move(value)), _result(std::move(result)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Operand _value, _result;
    Type _type;
};

}

std::ostream &operator<<(std::ostream &os, const il::Binary::Operator &op);
