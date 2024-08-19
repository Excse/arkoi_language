#ifndef ARKOI_LANGUAGE_INSTRUCTION_H
#define ARKOI_LANGUAGE_INSTRUCTION_H

#include <optional>
#include <utility>
#include <variant>
#include <memory>
#include <string>

#include "symbol_table.h"
#include "visitor.h"
#include "ast.h"

using Operand = std::variant<std::shared_ptr<Symbol>, long long>;

std::ostream &operator<<(std::ostream &os, const Operand &token);

class Instruction {
public:
    virtual ~Instruction() = default;

    virtual void accept(InstructionVisitor &visitor) const = 0;
};

struct LabelInstruction : public Instruction {
public:
    explicit LabelInstruction(std::string name) : _name(std::move(name)) {}

    void accept(InstructionVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const std::string &name() const { return _name; }

private:
    std::string _name;
};

struct ReturnInstruction : public Instruction {
public:
    explicit ReturnInstruction(Operand &&value) : _value(std::move(value)) {}

    void accept(InstructionVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Operand &value() const { return _value; };

private:
    Operand _value;
};

struct BinaryInstruction : public Instruction {
public:
    enum class Type {
        Add,
        Sub,
        Mul,
        Div,
    };

public:
    BinaryInstruction(Operand &&result, Operand &&left, Type type, Operand &&right)
        : _result(std::move(result)), _left(std::move(left)), _type(type), _right(std::move(right)) {}

    void accept(InstructionVisitor &visitor) const override { visitor.visit(*this); }

    [[nodiscard]] const Operand &result() const { return _result; };

    [[nodiscard]] const Operand &right() const { return _right; };

    [[nodiscard]] const Operand &left() const { return _left; };

    [[nodiscard]] const Type &type() const { return _type; };

    [[nodiscard]] static Type node_to_instruction(BinaryNode::Type type);

    [[nodiscard]] static std::string type_to_string(Type type);

private:
    Operand _result, _left, _right;
    Type _type;
};

#endif //ARKOI_LANGUAGE_INSTRUCTION_H
