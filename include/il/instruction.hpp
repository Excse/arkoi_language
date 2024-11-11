#pragma once

#include <utility>
#include <memory>

#include "utils/visitor.hpp"
#include "frontend/ast.hpp"
#include "il/operand.hpp"

namespace il {

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(il::Visitor &visitor) = 0;
};

class Label : public Instruction {
public:
    Label(SharedSymbol symbol) : _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    SharedSymbol _symbol;
};

class Goto : public Instruction {
public:
    Goto(SharedSymbol label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    SharedSymbol _label;
};

class If : public Instruction {
public:
    If(Operand condition, SharedSymbol label)
        : _condition(std::move(condition)), _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &condition() { return _condition; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    Operand _condition;
    SharedSymbol _label;
};

class Call : public Instruction {
public:
    Call(il::Variable result, SharedSymbol function, std::vector<Operand> &&arguments)
        : _arguments(std::move(arguments)), _result(std::move(result)), _function(std::move(function)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

    [[nodiscard]] auto &arguments() { return _arguments; };

    [[nodiscard]] auto &result() const { return _result; };

private:
    std::vector<Operand> _arguments;
    il::Variable _result;
    SharedSymbol _function;
};

class Return : public Instruction {
public:
    Return(Operand value, Type type) : _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &value() { return _value; };

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
    Binary(il::Variable result, Operand left, Operator op, Operand right, Type type)
        : _left(std::move(left)), _right(std::move(right)), _result(std::move(result)), _op(op),
          _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &right() const { return _right; };

    [[nodiscard]] auto &right() { return _right; };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] auto &left() { return _left; };

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(node::Binary::Operator op);

private:
    Operand _left, _right;
    il::Variable _result;
    Operator _op;
    Type _type;
};

class Begin : public Instruction {
public:
    Begin(SharedSymbol function) : _function(std::move(function)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

private:
    SharedSymbol _function;
};

class End : public Instruction {
public:
    void accept(Visitor &visitor) override { visitor.visit(*this); }
};

class Cast : public Instruction {
public:
    Cast(il::Variable result, Operand expression, Type from, Type to)
        : _result(std::move(result)), _expression(std::move(expression)), _from(from), _to(to) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; };

    [[nodiscard]] auto &expression() { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    il::Variable _result;
    Operand _expression;
    Type _from, _to;
};

class Store : public Instruction {
public:
    Store(il::Variable result, Operand value, Type type)
        : _result(std::move(result)), _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool has_side_effects() const;

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    il::Variable _result;
    Operand _value;
    Type _type;
};

struct InstructionType : std::variant<
    il::Label,
    il::Goto,
    il::If,
    il::Cast,
    il::Call,
    il::Return,
    il::Binary,
    il::Begin,
    il::End,
    il::Store
> {
    using variant::variant;

    void accept(il::Visitor &visitor);
};

}

std::ostream &operator<<(std::ostream &os, const il::Binary::Operator &op);
