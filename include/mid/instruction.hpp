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
    Call(mid::Variable result, mid::Variable function, std::vector<Operand> &&arguments)
        : _result(std::move(result)), _function(std::move(function)), _arguments(std::move(arguments)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &function() const { return _function; }

    [[nodiscard]] auto &arguments() { return _arguments; };

    [[nodiscard]] auto &result() const { return _result; };

private:
    mid::Variable _result, _function;
    std::vector<Operand> _arguments;
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
        GreaterThan,
        LessThan,
    };

public:
    Binary(mid::Variable result, Operand left, Operator op, Operand right, Type op_type, Type result_type)
        : _result_type(result_type), _op_type(op_type), _left(std::move(left)), _right(std::move(right)),
          _result(std::move(result)), _op(op) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &right() const { return _right; };

    [[nodiscard]] auto &right() { return _right; };

    [[nodiscard]] auto &left() const { return _left; };

    [[nodiscard]] auto &left() { return _left; };

    [[nodiscard]] auto &result_type() const { return _result_type; };

    [[nodiscard]] auto &op_type() const { return _op_type; };

    [[nodiscard]] auto &op() const { return _op; };

    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    Type _result_type, _op_type;
    Operand _left, _right;
    mid::Variable _result;
    Operator _op;
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

class Alloca : public Instruction {
public:
    Alloca(mid::Variable result, Type type)
        : _result(std::move(result)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    mid::Variable _result;
    Type _type;
};

class Load : public Instruction {
public:
    Load(mid::Variable result, mid::Variable target, Type type)
        : _result(std::move(result)), _target(std::move(target)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] auto &result() const { return _result; };

    [[nodiscard]] auto &target() const { return _target; };

    [[nodiscard]] auto &type() const { return _type; };

private:
    mid::Variable _result, _target;
    Type _type;
};

class Store : public Instruction {
public:
    Store(mid::Variable result, Operand value, Type type)
        : _result(std::move(result)), _value(std::move(value)), _type(type) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

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
    mid::Alloca,
    mid::Store,
    mid::Load
> {
    using variant::variant;

    void accept(mid::Visitor &visitor);
};

} // namespace arkoi::mid

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Binary::Operator &op);

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
