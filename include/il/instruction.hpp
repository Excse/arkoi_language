#pragma once

#include <utility>

#include "il/visitor.hpp"
#include "il/operand.hpp"
#include "ast/nodes.hpp"

namespace arkoi::il {

class InstructionBase {
public:
    virtual ~InstructionBase() = default;

    virtual void accept(Visitor &visitor) = 0;

    [[nodiscard]] virtual bool is_constant() = 0;
};

class Goto : public InstructionBase {
public:
    Goto(std::string label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    void set_label(std::string label) { _label = std::move(label); }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::string _label;
};

class If : public InstructionBase {
public:
    If(Operand condition, std::string next, std::string branch)
        : _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_condition); }

    [[nodiscard]] auto &condition() { return _condition; }

    [[nodiscard]] auto &branch() const { return _branch; }

    [[nodiscard]] auto &next() const { return _next; }

private:
    std::string _next, _branch;
    Operand _condition;
};

class Call : public InstructionBase {
public:
    Call(Variable result, std::string name, std::vector<Operand> &&arguments, Type type)
        : _arguments(std::move(arguments)), _result(std::move(result)), _name(std::move(name)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &arguments() { return _arguments; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &name() const { return _name; }

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::vector<Operand> _arguments;
    Variable _result;
    std::string _name;
    Type _type;
};

class Return : public InstructionBase {
public:
    Return(Operand value, Type type) : _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &value() { return _value; };

private:
    Operand _value;
    Type _type;
};

class Binary : public InstructionBase {
public:
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
        GreaterThan,
        LessThan,
    };

public:
    Binary(Variable result, Operand left, Operator op, Operand right, Type op_type, Type result_type)
        : _result_type(result_type), _op_type(op_type), _left(std::move(left)), _right(std::move(right)),
          _result(std::move(result)), _op(op) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override {
        return std::holds_alternative<Immediate>(_left) &&
               std::holds_alternative<Immediate>(_right);
    }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &right() { return _right; };

    [[nodiscard]] auto &left() { return _left; };

    [[nodiscard]] auto &result_type() const { return _result_type; };

    [[nodiscard]] auto &op_type() const { return _op_type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    Type _result_type, _op_type;
    Operand _left, _right;
    Variable _result;
    Operator _op;
};

class Cast : public InstructionBase {
public:
    Cast(Variable result, Operand expression, Type from, Type to)
        : _result(std::move(result)), _expression(std::move(expression)), _from(from), _to(to) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_expression); }

    [[nodiscard]] auto &expression() { return _expression; };

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &from() const { return _from; };

    [[nodiscard]] auto &to() const { return _to; };

private:
    Variable _result;
    Operand _expression;
    Type _from, _to;
};

class Alloca : public InstructionBase {
public:
    Alloca(Variable result, Type type)
        : _result(std::move(result)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Variable _result;
    Type _type;
};

class Load : public InstructionBase {
public:
    Load(Variable result, Variable target, Type type)
        : _result(std::move(result)), _target(std::move(target)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &target() const { return _target; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    Variable _result, _target;
    Type _type;
};

class Store : public InstructionBase {
public:
    Store(Variable result, Operand value, Type type)
        : _result(std::move(result)), _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &type() const { return _type; };

    [[nodiscard]] auto &value() { return _value; };

private:
    Variable _result;
    Operand _value;
    Type _type;
};

class Constant : public InstructionBase {
public:
    Constant(Variable result, Immediate value, Type type)
        : _result(std::move(result)), _value(value), _type(std::move(type)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return true; }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &value() { return _value; };

    [[nodiscard]] auto &type() { return _type; };

private:
    Variable _result;
    Immediate _value;
    Type _type;
};

struct Instruction : std::variant<
    il::Goto,
    il::If,
    il::Cast,
    il::Call,
    il::Return,
    il::Binary,
    il::Alloca,
    il::Store,
    il::Load,
    il::Constant
> {
    using variant::variant;

    void accept(il::Visitor &visitor);

    [[nodiscard]] bool is_constant();
};

} // namespace arkoi::mid

std::ostream &operator<<(std::ostream &os, const arkoi::il::Binary::Operator &op);

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
