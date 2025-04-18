#pragma once

#include <utility>

#include "ast/nodes.hpp"
#include "il/operand.hpp"
#include "il/visitor.hpp"

namespace arkoi::il {

class InstructionBase {
public:
    virtual ~InstructionBase() = default;

    virtual void accept(Visitor &visitor) = 0;

    [[nodiscard]] virtual std::vector<Operand> defs() const { return {}; }

    [[nodiscard]] virtual std::vector<Operand> uses() const { return {}; }

    [[nodiscard]] virtual bool is_constant() = 0;
};

class Goto final : public InstructionBase {
public:
    explicit Goto(std::string label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::string _label;
};

class If final : public InstructionBase {
public:
    If(Operand condition, std::string next, std::string branch)
        : _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> uses() const override { return {_condition}; }

    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_condition); }

    [[nodiscard]] auto &condition() { return _condition; }

    [[nodiscard]] auto &branch() const { return _branch; }

    [[nodiscard]] auto &next() const { return _next; }

private:
    std::string _next, _branch;
    Operand _condition;
};

class Call final : public InstructionBase {
public:
    Call(Variable result, std::string name, std::vector<Operand> &&arguments)
        : _arguments(std::move(arguments)), _name(std::move(name)), _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return {_result}; }

    [[nodiscard]] std::vector<Operand> uses() const override { return _arguments; }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &arguments() { return _arguments; }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::vector<Operand> _arguments;
    std::string _name;
    Variable _result;
};

class Return final : public InstructionBase {
public:
    explicit Return(Operand value) : _value(std::move(value)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> uses() const override { return {_value}; }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &value() { return _value; }

private:
    Operand _value;
};

class Binary final : public InstructionBase {
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
    Binary(Variable result, Operand left, Operator op, Operand right, sem::Type op_type)
        : _left(std::move(left)), _right(std::move(right)), _result(std::move(result)),
          _op_type(std::move(op_type)), _op(op) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return {_result}; }

    [[nodiscard]] std::vector<Operand> uses() const override { return {_left, _right}; }

    [[nodiscard]] bool is_constant() override {
        return std::holds_alternative<Immediate>(_left) && std::holds_alternative<Immediate>(_right);
    }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &right() { return _right; }

    [[nodiscard]] auto &left() { return _left; }

    [[nodiscard]] auto &op_type() const { return _op_type; }

    [[nodiscard]] auto &op() const { return _op; }

    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    Operand _left, _right;
    Variable _result;
    sem::Type _op_type;
    Operator _op;
};

class Cast final : public InstructionBase {
public:
    Cast(Variable result, Operand source, sem::Type from)
        : _result(std::move(result)), _source(std::move(source)), _from(std::move(from)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_source); }

    [[nodiscard]] auto &source() { return _source; }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &from() const { return _from; }

private:
    Variable _result;
    Operand _source;
    sem::Type _from;
};

class Alloca final : public InstructionBase {
public:
    explicit Alloca(Memory result) : _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; }

private:
    Memory _result;
};

class Load final : public InstructionBase {
public:
    Load(Variable result, Memory source)
        : _result(std::move(result)), _source(std::move(source)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &source() const { return _source; }

private:
    Variable _result;
    Memory _source;
};

class Store final : public InstructionBase {
public:
    Store(Memory result, Operand source)
        : _result(std::move(result)), _source(std::move(source)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    [[nodiscard]] bool is_constant() override { return false; }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &source() { return _source; }

private:
    Memory _result;
    Operand _source;
};

class Constant final : public InstructionBase {
public:
    Constant(Variable result, Immediate immediate)
        : _immediate(std::move(std::move(immediate))), _result(std::move(result)) {}

    void accept(Visitor &visitor) override { visitor.visit(*this); }

    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    [[nodiscard]] std::vector<Operand> uses() const override { return { _immediate }; }

    [[nodiscard]] bool is_constant() override { return true; }

    [[nodiscard]] auto &result() const { return _result; }

    [[nodiscard]] auto &immediate() { return _immediate; }

private:
    Immediate _immediate;
    Variable _result;
};

struct Instruction final : InstructionBase, std::variant<
    Goto, If, Cast, Call, Return,
    Binary, Alloca, Store, Load, Constant
> {
    using variant::variant;

    void accept(Visitor &visitor) override;

    [[nodiscard]] std::vector<Operand> defs() const override;

    [[nodiscard]] std::vector<Operand> uses() const override;

    [[nodiscard]] bool is_constant() override;
};

} // namespace arkoi::il

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
