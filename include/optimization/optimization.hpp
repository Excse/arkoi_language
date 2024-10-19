#pragma once

#include "il/cfg.hpp"

class IterativePass {
public:
    virtual ~IterativePass() = default;

    virtual bool new_cfg(CFG &cfg) = 0;

    virtual bool new_block(BasicBlock &block) = 0;
};

class SinglePass {
public:
    virtual ~SinglePass() = default;

    virtual void new_cfg(CFG &cfg) = 0;

    virtual void new_block(BasicBlock &block) = 0;
};

class OptimizationManager {
public:
    void optimize(std::vector<CFG> &cfgs);

    template<typename SingleType, typename... Args>
    SingleType &emplace_single(Args &&... args);

    template<typename IterativeType, typename... Args>
    IterativeType &emplace_iterative(Args &&... args);

private:
    std::vector<std::unique_ptr<IterativePass>> _iterative_passes;
    std::vector<std::unique_ptr<SinglePass>> _single_passes;
};

#include "../../src/optimization/optimization.tpp"
