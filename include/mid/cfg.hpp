#pragma once

#include <functional>
#include <utility>
#include <vector>
#include <memory>

#include "mid/instruction.hpp"

namespace arkoi::mid {

class BasicBlock {
public:
    BasicBlock(SharedSymbol symbol) : _symbol(std::move(symbol)) {}

    void accept(mid::Visitor &visitor) { visitor.visit(*this); }

    template<typename Type, typename... Args>
    mid::InstructionType &emplace(Args &&... args);

    [[nodiscard]] auto &instructions() { return _instructions; }

    void set_branch(std::shared_ptr<BasicBlock> branch) { _branch = std::move(branch); }

    [[nodiscard]] auto &branch() const { return _branch; }

    void set_next(std::shared_ptr<BasicBlock> next) { _next = std::move(next); }

    [[nodiscard]] auto &next() const { return _next; }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    std::vector<mid::InstructionType> _instructions{};
    std::shared_ptr<BasicBlock> _next{}, _branch{};
    SharedSymbol _symbol;
};

class Function {
public:
    Function(SharedSymbol symbol, std::shared_ptr<BasicBlock> start, std::shared_ptr<BasicBlock> end)
        : _start(std::move(start)), _end(std::move(end)), _symbol(std::move(symbol)) {}

    void accept(mid::Visitor &visitor) { visitor.visit(*this); }

    void depth_first_search(const std::function<void(BasicBlock &)> &callback);

    void linearize(const std::function<void(mid::InstructionType &)> &callback);

    [[nodiscard]] auto &symbol() const { return _symbol; }

    [[nodiscard]] auto &start() const { return _start; }

    [[nodiscard]] auto &end() const { return _end; }

private:
    std::shared_ptr<BasicBlock> _start{}, _end{};
    SharedSymbol _symbol;
};

class Module {
public:
    void accept(mid::Visitor &visitor) { visitor.visit(*this); }

    [[nodiscard]] auto &functions() { return _functions; }

private:
    std::vector<Function> _functions{};
};

#include "../../src/mid/cfg.tpp"

} // namespace arkoi::mid