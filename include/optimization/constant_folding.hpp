#pragma once

#include "optimization.hpp"

class ConstantFolding : public Pass {
public:
    bool new_function(Function &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<il::InstructionType> _binary(const il::Binary &instruction);

    [[nodiscard]] static il::Operand _evaluate_binary(const il::Binary &instruction, auto left, auto right);

    [[nodiscard]] static std::optional<il::InstructionType> _cast(const il::Cast &instruction);

    [[nodiscard]] static il::Operand _evaluate_unary(const Type &to, auto expression);
};
