#pragma once

#include "optimization.hpp"

namespace arkoi {

class ConstantFolding : public IterativePass {
public:
    bool new_cfg(CFG &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<std::unique_ptr<intermediate::Instruction>> _binary(const intermediate::BinaryInstruction &instruction);

    [[nodiscard]] static std::optional<std::unique_ptr<intermediate::Instruction>> _cast(const intermediate::CastInstruction &instruction);
};

}