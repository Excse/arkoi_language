#pragma once

#include <unordered_set>
#include <functional>
#include <utility>
#include <vector>
#include <memory>
#include <stack>

#include "il/instruction.hpp"

namespace arkoi::il {

class Function;

class BasicBlock {
public:
    using Instructions = std::vector<Instruction>;

public:
    BasicBlock(std::string label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) { visitor.visit(*this); }

    template<typename Type, typename... Args>
    Instruction &emplace_back(Args &&... args);

    [[nodiscard]] auto &instructions() { return _instructions; }

    void set_branch(std::shared_ptr<BasicBlock> branch) { _branch = std::move(branch); }

    [[nodiscard]] auto &branch() const { return _branch; }

    void set_next(std::shared_ptr<BasicBlock> next) { _next = std::move(next); }

    [[nodiscard]] auto &next() const { return _next; }

    [[nodiscard]] auto &label() const { return _label; }

    Instructions::iterator begin() { return _instructions.begin(); }

    Instructions::iterator end() { return _instructions.end(); }

private:
    std::shared_ptr<BasicBlock> _next{}, _branch{};
    Instructions _instructions{};
    std::string _label;
};

class BlockIterator {
public:
    using iterator_category [[maybe_unused]] = std::input_iterator_tag;
    using value_type = BasicBlock;
    using reference = value_type &;
    using pointer = value_type *;

public:
    BlockIterator(Function *function);

    reference operator*() const { return *_current; }

    pointer operator->() const { return _current; }

    BlockIterator &operator++();

    BlockIterator operator++(int);

    friend bool operator==(const BlockIterator &left, const BlockIterator &right) {
        return left._current == right._current;
    }

    friend bool operator!=(const BlockIterator &left, const BlockIterator &right) {
        return left._current != right._current;
    }

private:
    std::unordered_set<BasicBlock *> _visited{};
    std::stack<BasicBlock *> _queue;
    Function *_function;
    pointer _current;
};

class Function {
public:
    Function(std::shared_ptr<Symbol> symbol, std::shared_ptr<BasicBlock> entry, std::shared_ptr<BasicBlock> exit)
        : _entry(std::move(entry)), _exit(std::move(exit)), _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) { visitor.visit(*this); }

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &entry() const { return _entry; }

    [[nodiscard]] auto &exit() const { return _exit; }

    BlockIterator begin() { return {this}; }

    BlockIterator end() { return {nullptr}; }

private:
    std::shared_ptr<BasicBlock> _entry{}, _exit{};
    std::shared_ptr<Symbol> _symbol;
};

class Module {
public:
    using Functions = std::vector<Function>;

public:
    void accept(Visitor &visitor) { visitor.visit(*this); }

    template<typename... Args>
    Function &emplace_back(Args &&... args);

    Functions::iterator begin() { return _functions.begin(); }

    Functions::iterator end() { return _functions.end(); }

private:
    Functions _functions{};
};

#include "../../src/il/cfg.tpp"

} // namespace arkoi::mid

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
