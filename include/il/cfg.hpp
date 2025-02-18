#pragma once

#include <functional>
#include <utility>
#include <vector>
#include <memory>

#include "il/instruction.hpp"

namespace arkoi::il {

class BasicBlock {
public:
    BasicBlock(std::string label) : _label(std::move(label)) {}

    void accept(Visitor &visitor) { visitor.visit(*this); }

    template<typename Type, typename... Args>
    void add(Args &&... args);

    [[nodiscard]] auto &instructions() { return _instructions; }

    void set_branch(std::shared_ptr<BasicBlock> branch) { _branch = std::move(branch); }

    [[nodiscard]] auto &branch() const { return _branch; }

    void set_next(std::shared_ptr<BasicBlock> next) { _next = std::move(next); }

    [[nodiscard]] auto &next() const { return _next; }

    [[nodiscard]] auto &label() const { return _label; }

private:
    std::shared_ptr<BasicBlock> _next{}, _branch{};
    std::vector<Instruction> _instructions{};
    std::string _label;
};

class Function {
public:
    Function(std::shared_ptr<Symbol> symbol, std::shared_ptr<BasicBlock> start, std::shared_ptr<BasicBlock> end)
        : _start(std::move(start)), _end(std::move(end)), _symbol(std::move(symbol)) {}

    void accept(Visitor &visitor) { visitor.visit(*this); }

    void depth_first_search(const std::function<void(BasicBlock &)> &callback);

    void linearize(const std::function<void(Instruction &)> &callback);

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &start() const { return _start; }

    [[nodiscard]] auto &end() const { return _end; }

private:
    std::shared_ptr<BasicBlock> _start{}, _end{};
    std::shared_ptr<Symbol> _symbol;
};

class Module {
public:
    void accept(Visitor &visitor) { visitor.visit(*this); }

    [[nodiscard]] auto &functions() { return _functions; }

private:
    std::vector<Function> _functions{};
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
