#pragma once

#include "cfg.hpp"

class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;

    virtual void new_cfg(CFG &cfg) = 0;

    virtual void new_block(BasicBlock &block) = 0;

    virtual void instruction(Instruction &instruction) = 0;
};

class OptimizationManager {
public:
    void optimize(std::vector<CFG> &cfgs);

    void push_back(std::unique_ptr<OptimizationPass> pass);

private:
    std::vector<std::unique_ptr<OptimizationPass>> _passes;
};