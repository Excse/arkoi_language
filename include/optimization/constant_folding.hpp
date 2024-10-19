#pragma once

#include "optimization.hpp"

class ConstantFolding : public IterativePass {
public:
    bool new_cfg(CFG &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _binary(const il::Binary &instruction);

    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _cast(const il::Cast &instruction);
};
