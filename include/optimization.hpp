#pragma once

#include "cfg.hpp"

class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;

    virtual void new_cfg(CFG &cfg) = 0;

    virtual void new_block(BasicBlock &block) = 0;
};

class OptimizationManager {
public:
    void optimize(std::vector<CFG> &cfgs);

    template<typename OptimizationType, typename... Args>
    OptimizationType &emplace_back(Args &&... args);

private:
    std::vector<std::unique_ptr<OptimizationPass>> _passes;
};

#include "../src/optimization.tpp"