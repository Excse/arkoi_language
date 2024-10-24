#pragma once

#include <unordered_set>
#include <vector>
#include <memory>

#include "instruction.hpp"

class BasicBlock {
public:
    template<typename InstructionType, typename... Args>
    void emplace_back(Args &&... args);

    void depth_first_search(const std::function<void(BasicBlock &)> &callback,
                            std::unordered_set<BasicBlock *> &visited);

    [[nodiscard]] auto &instructions() { return _instructions; }

    void set_branch(std::shared_ptr<BasicBlock> branch) { _branch = std::move(branch); }

    [[nodiscard]] auto &branch() const { return _branch; }

    void set_next(std::shared_ptr<BasicBlock> next) { _next = std::move(next); }

    [[nodiscard]] auto &next() const { return _next; }

private:
    std::vector<std::unique_ptr<Instruction>> _instructions{};
    std::shared_ptr<BasicBlock> _next{}, _branch{};
};

class Function {
public:
    Function(std::shared_ptr<BasicBlock> start, std::shared_ptr<BasicBlock> end)
        : _start(std::move(start)), _end(std::move(end)) {}

    void depth_first_search(const std::function<void(BasicBlock &)> &callback);

    void linearize(const std::function<void(Instruction &)> &callback);

    [[nodiscard]] auto &start() const { return _start; }

    [[nodiscard]] auto &end() const { return _end; }

private:
    std::shared_ptr<BasicBlock> _start{}, _end{};
};

#include "../../src/il/cfg.tpp"
