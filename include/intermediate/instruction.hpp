#pragma once

#include <optional>
#include <utility>
#include <memory>
#include <string>

#include "semantic/symbol_table.hpp"
#include "operand.hpp"
#include "utils/visitor.hpp"
#include "frontend/ast.hpp"

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(intermediate::Visitor &visitor) = 0;
};

namespace intermediate {

class Label : public Instruction {
public:
    explicit Label(std::shared_ptr<Symbol> symbol) : _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
};

class Goto : public Instruction {
public:
    explicit Goto(std::shared_ptr<Symbol> label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::shared_ptr<Symbol> _label;
};

class IfNot : public Instruction {
public:
    IfNot(Operand condition, std::shared_ptr<Symbol> label)
        : _label(std::move(label)), _condition(std::move(condition)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_condition(Operand condition) { _condition = std::move(condition); }

    [[nodiscard]] auto &condition() const { return _condition; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::shared_ptr<Symbol> _label;
    Operand _condition;
};

class Call : public Instruction {
public:
    explicit Call(Operand result, std::shared_ptr<Symbol> symbol)
        : _symbol(std::move(symbol)), _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_result(Operand operand) { _result = std::move(operand); };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
    Operand _result;
};

class Return : public Instruction {
public:
    explicit Return(Operand value, Type type) : _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; };

    void set_value(Operand operand) { _value = std::move(operand); };

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

    void set_result(Operand operand) { _result = std::move(operand); };

    [[nodiscard]] auto &result() const { return _result; };

    void set_right(Operand operand) { _right = std::move(operand); };

    [[nodiscard]] auto &right() const { return _right; };

    void set_left(Operand operand) { _left = std::move(operand); };

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
    explicit Begin(std::shared_ptr<Symbol> label, int64_t local_size = 0)
        : _label(std::move(label)), _local_size(local_size) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void increase_local_size(int64_t amount) { _local_size += amount; }

    void set_local_size(int64_t size) { _local_size = size; }

    [[nodiscard]] auto local_size() const { return _local_size; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::shared_ptr<Symbol> _label;
    int64_t _local_size;
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

    void set_expression(Operand operand) { _expression = std::move(operand); };

    [[nodiscard]] auto &expression() const { return _expression; };

    void set_result(Operand operand) { _result = std::move(operand); };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    Operand _result, _expression;
    Type _from, _to;
};

class Argument : public Instruction {
public:
    explicit Argument(Operand expression, std::shared_ptr<Symbol> symbol)
        : _symbol(std::move(symbol)), _expression(std::move(expression)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_expression(Operand operand) { _expression = std::move(operand); };

    [[nodiscard]] auto &expression() const { return _expression; };

    void set_result(Register reg) { _result = reg; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::optional<Operand> _result{};
    std::shared_ptr<Symbol> _symbol;
    Operand _expression;
};

class Store : public Instruction {
public:
    explicit Store(Operand result, Operand value, Type type)
        : _value(std::move(value)), _result(std::move(result)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    void set_value(Operand operand) { _value = std::move(operand); };

    [[nodiscard]] auto &value() const { return _value; };

    void set_result(Operand result) { _result = std::move(result); };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Operand _value, _result;
    Type _type;
};

}

std::ostream &operator<<(std::ostream &os, const intermediate::Binary::Operator &op);
