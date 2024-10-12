#pragma once

#include "optimization.hpp"

class ConstantFolding : public OptimizationPass {
public:
    void new_cfg(CFG &) override {}

    void new_block(BasicBlock &) override {}

    void instruction(Instruction &instruction) override;

private:
    [[nodiscard]] static Instruction _binary(const BinaryInstruction &instruction);

    [[nodiscard]] static Instruction _cast(const CastInstruction &instruction);
};
