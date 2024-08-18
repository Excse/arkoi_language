//
// Created by timo on 8/18/24.
//

#ifndef ARKOI_LANGUAGE_TAC_H
#define ARKOI_LANGUAGE_TAC_H

#include <optional>
#include <utility>
#include <variant>
#include <memory>
#include <string>

#include "symbol_table.h"
#include "visitor.h"

using Operand = std::variant<std::shared_ptr<Symbol>, long long>;

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

#endif //ARKOI_LANGUAGE_TAC_H
