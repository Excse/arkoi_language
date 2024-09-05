#ifndef ARKOI_LANGUAGE_INSTRUCTION_H
#define ARKOI_LANGUAGE_INSTRUCTION_H

#include <optional>
#include <utility>
#include <variant>
#include <memory>
#include <string>

#include "symbol_table.h"
#include "operand.h"
#include "visitor.h"
#include "ast.h"

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(InstructionVisitor &visitor) = 0;
};

class LabelInstruction : public Instruction {
public:
    explicit LabelInstruction(std::shared_ptr<Symbol> symbol) : _symbol(std::move(symbol)) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
};

class ReturnInstruction : public Instruction {
public:
    explicit ReturnInstruction(Operand &&value) : _value(std::move(value)) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void set_value(Operand &&operand) { _value = operand; };

    [[nodiscard]] auto &value() const { return _value; };

private:
    Operand _value;
};

class BinaryInstruction : public Instruction {
public:
    enum class Type {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryInstruction(Operand &&result, Operand &&left, Type type, Operand &&right)
            : _result(std::move(result)), _left(std::move(left)), _right(std::move(right)), _type(type) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; };

    void set_result(Operand &&operand) { _result = operand; };

    [[nodiscard]] auto &result() const { return _result; };

    void set_right(Operand &&operand) { _right = operand; };

    [[nodiscard]] auto &right() const { return _right; };

    void set_left(Operand &&operand) { _left = operand; };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] static Type node_to_instruction(BinaryNode::Type type);

private:
    Operand _result, _left, _right;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Type &type);

class BeginInstruction : public Instruction {
public:
    explicit BeginInstruction(int64_t size = 0) : _size(size) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void increase_size(int64_t amount) { _size += amount; }

    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _size;
};

class EndInstruction : public Instruction {
public:
    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }
};

class CastInstruction : public Instruction {
public:
    CastInstruction(Operand &&result, const std::shared_ptr<Type> &type, Operand &&expression)
            : _result(std::move(result)), _expression(std::move(expression)), _type(type) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void set_expression(Operand &&operand) { _expression = operand; };

    [[nodiscard]] auto &expression() const { return _expression; };

    void set_result(Operand &&operand) { _result = operand; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Operand _result, _expression;
    std::shared_ptr<Type> _type;
};

#endif //ARKOI_LANGUAGE_INSTRUCTION_H
