#pragma once

#include <utility>
#include <memory>

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
    Label(Symbol symbol) : _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    Symbol _symbol;
};

class Goto : public Instruction {
public:
    Goto(Symbol label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Symbol _label;
};

class If : public Instruction {
public:
    If(Operand condition, Symbol label)
        : _condition(std::move(condition)), _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_condition(Operand condition) { _condition = std::move(condition); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Operand _condition;
    Symbol _label;
};

class Call : public Instruction {
public:
    Call(Symbol result, Symbol function, std::vector<Operand> &&arguments)
        : _arguments(std::move(arguments)), _result(std::move(result)), _function(std::move(function)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

    [[nodiscard]] auto &arguments() { return _arguments; };

    [[nodiscard]] auto &result() const { return _result; };

private:
    std::vector<Operand> _arguments;
    Symbol _result, _function;
};

class Return : public Instruction {
public:
    Return(Operand value, Type type) : _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_value(Operand value) { _value = std::move(value); };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &type() const { return _type; };

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
    Binary(Symbol result, Operand left, Operator op, Operand right, Type type)
        : _left(std::move(left)), _right(std::move(right)), _result(std::move(result)), _op(op),
          _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    void set_right(Operand right) { _right = std::move(right); };

    [[nodiscard]] auto &right() const { return _right; };

    void set_left(Operand left) { _left = std::move(left); };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(node::Binary::Operator op);

private:
    Operand _left, _right;
    Symbol _result;
    Operator _op;
    Type _type;
};

class Begin : public Instruction {
public:
    Begin(Symbol function) : _function(std::move(function)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

private:
    Symbol _function;
};

class End : public Instruction {
public:
    void accept(Visitor &visitor) override { visitor.visit(*this); }
};

class Cast : public Instruction {
public:
    Cast(Symbol result, Operand expression, Type from, Type to)
        : _expression(std::move(expression)), _from(from), _to(to), _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_expression(Operand expression) { _expression = std::move(expression); };

    [[nodiscard]] auto &expression() const { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    Operand _expression;
    Type _from, _to;
    Symbol _result;
};

class Store : public Instruction {
public:
    Store(Symbol result, Operand value, Type type)
        : _result(std::move(result)), _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Symbol _result;
    Operand _value;
    Type _type;
};

}

std::ostream &operator<<(std::ostream &os, const il::Binary::Operator &op);
