#pragma once

#include "optimization.hpp"

namespace arkoi {

class ConstantFolding : public IterativePass {
public:
    bool new_cfg(CFG &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _binary(const intermediate::Binary &instruction);

    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _cast(const intermediate::Cast &instruction);
};

}