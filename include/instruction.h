#ifndef ARKOI_LANGUAGE_INSTRUCTION_H
#define ARKOI_LANGUAGE_INSTRUCTION_H

#include <optional>
#include <utility>
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
    explicit ReturnInstruction(std::shared_ptr<Operand> value, std::shared_ptr<Type> type)
            : _value(std::move(value)), _type(std::move(type)) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &type() const { return _type; };

    void set_value(std::shared_ptr<Operand> operand) { _value = std::move(operand); };

    [[nodiscard]] auto &value() const { return _value; };

private:
    std::shared_ptr<Operand> _value;
    std::shared_ptr<Type> _type;
};

class BinaryInstruction : public Instruction {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryInstruction(std::shared_ptr<Operand> result, std::shared_ptr<Operand> left, Operator op,
                      std::shared_ptr<Operand> right, std::shared_ptr<Type> type)
            : _result(std::move(result)), _left(std::move(left)), _right(std::move(right)), _type(std::move(type)),
              _op(op) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void set_result(std::shared_ptr<Operand> operand) { _result = std::move(operand); };

    [[nodiscard]] auto &result() const { return _result; };

    void set_right(std::shared_ptr<Operand> operand) { _right = std::move(operand); };

    [[nodiscard]] auto &right() const { return _right; };

    void set_left(std::shared_ptr<Operand> operand) { _left = std::move(operand); };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(BinaryNode::Operator op);

    friend std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Operator &op);

private:
    std::shared_ptr<Operand> _result, _left, _right;
    std::shared_ptr<Type> _type;
    Operator _op;
};

class BeginInstruction : public Instruction {
public:
    explicit BeginInstruction(int64_t local_size = 0) : _local_size(local_size) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void increase_local_size(int64_t amount) { _local_size += amount; }

    [[nodiscard]] auto local_size() const { return _local_size; }

private:
    int64_t _local_size;
};

class EndInstruction : public Instruction {
public:
    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }
};

class CastInstruction : public Instruction {
public:
    CastInstruction(std::shared_ptr<Operand> result, std::shared_ptr<Operand> expression, std::shared_ptr<Type> from,
                    std::shared_ptr<Type> to)
            : _result(std::move(result)), _expression(std::move(expression)), _from(std::move(from)),
              _to(std::move(to)) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    void set_expression(std::shared_ptr<Operand> operand) { _expression = std::move(operand); };

    [[nodiscard]] auto &expression() const { return _expression; };

    void set_result(std::shared_ptr<Operand> operand) { _result = std::move(operand); };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    std::shared_ptr<Operand> _result, _expression;
    std::shared_ptr<Type> _from, _to;
};

#endif //ARKOI_LANGUAGE_INSTRUCTION_H
