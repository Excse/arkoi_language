#pragma once

#include "mid/cfg.hpp"

namespace arkoi::opt {

class Pass {
public:
    virtual ~Pass() = default;

    virtual bool new_function(mid::Function &function) = 0;

    virtual bool new_block(mid::BasicBlock &block) = 0;
};

class PassManager {
public:
    void optimize(mid::Module &module);

    template<typename Type, typename... Args>
    Type &emplace(Args &&... args);

private:
    std::vector<std::unique_ptr<Pass>> _passes;
};

#include "../../src/opt/optimization.tpp"

} // namespace arkoi::opt