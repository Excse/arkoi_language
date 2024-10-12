#pragma once

#include "optimization.hpp"

class ConstantFolding : public OptimizationPass {
public:
    void new_cfg(CFG &) override {}

    void new_block(BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _binary(const BinaryInstruction &instruction);

    [[nodiscard]] static std::optional<std::unique_ptr<Instruction>> _cast(const CastInstruction &instruction);
};
