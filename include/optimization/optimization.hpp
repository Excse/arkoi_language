#pragma once

#include "il/cfg.hpp"

class Pass {
public:
    virtual ~Pass() = default;

    virtual bool new_function(Function &function) = 0;

    virtual bool new_block(BasicBlock &block) = 0;
};

class OptimizationManager {
public:
    void optimize(std::vector<Function> &functions);

    template<typename IterativeType, typename... Args>
    IterativeType &emplace(Args &&... args);

private:
    std::vector<std::unique_ptr<Pass>> _passes;
};

#include "../../src/optimization/optimization.tpp"
