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

    [[nodiscard]] const auto &symbol() const { return _symbol; }

private:
    std::shared_ptr<Symbol> _symbol;
};

class ReturnInstruction : public Instruction {
public:
    explicit ReturnInstruction(Operand &&value) : _value(std::move(value)) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &value() { return _value; };

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

    [[nodiscard]] const auto &type() const { return _type; };

    [[nodiscard]] auto &result()  { return _result; };

    [[nodiscard]] auto &right()  { return _right; };

    [[nodiscard]] auto &left() { return _left; };

    [[nodiscard]] static Type node_to_instruction(BinaryNode::Type type);

private:
    Operand _result, _left, _right;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Type &type);

class BeginInstruction : public Instruction {
public:
    explicit BeginInstruction(size_t size = 0) : _size(size) {}

    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &size() { return _size; }

private:
    size_t _size;
};

class EndInstruction : public Instruction {
public:
    void accept(InstructionVisitor &visitor) override { visitor.visit(*this); }
};

#endif //ARKOI_LANGUAGE_INSTRUCTION_H
