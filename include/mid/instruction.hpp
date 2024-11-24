#pragma once

#include <utility>
#include <memory>

#include "utils/visitor.hpp"
#include "mid/operand.hpp"
#include "def/ast.hpp"

namespace arkoi::mid {

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(mid::Visitor &visitor) = 0;
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
    Call(mid::Variable result, SharedSymbol function, std::vector<Operand> &&arguments)
        : _arguments(std::move(arguments)), _function(std::move(function)), _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

    [[nodiscard]] auto &arguments() { return _arguments; };

    [[nodiscard]] auto &result() const { return _result; };

private:
    std::vector<Operand> _arguments;
    SharedSymbol _function;
    mid::Variable _result;
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
    Binary(mid::Variable result, Operand left, Operator op, Operand right, Type type)
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

    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    Operand _left, _right;
    mid::Variable _result;
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
    Cast(mid::Variable result, Operand expression, Type from, Type to)
        : _result(std::move(result)), _expression(std::move(expression)), _from(from), _to(to) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &expression() const { return _expression; };

    [[nodiscard]] auto &expression() { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    mid::Variable _result;
    Operand _expression;
    Type _from, _to;
};

class Store : public Instruction {
public:
    Store(mid::Variable result, Operand value, Type type)
        : _result(std::move(result)), _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool has_side_effects() const;

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &value() const { return _value; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    mid::Variable _result;
    Operand _value;
    Type _type;
};

struct InstructionType : std::variant<
    mid::Label,
    mid::Goto,
    mid::If,
    mid::Cast,
    mid::Call,
    mid::Return,
    mid::Binary,
    mid::Begin,
    mid::End,
    mid::Store
> {
    using variant::variant;

    void accept(mid::Visitor &visitor);
};

} // namespace arkoi::mid

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Binary::Operator &op);
