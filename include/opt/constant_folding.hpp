#pragma once

#include "opt/optimization.hpp"

namespace arkoi::opt {

class ConstantFolding : public Pass {
public:
    bool new_function(mid::Function &) override { return false; }

    bool new_block(mid::BasicBlock &block) override;

private:
    [[nodiscard]] static std::optional<mid::InstructionType> _binary(const mid::Binary &instruction);

    [[nodiscard]] static mid::Operand _evaluate_binary(const mid::Binary &instruction, auto left, auto right);

    [[nodiscard]] static std::optional<mid::InstructionType> _cast(const mid::Cast &instruction);

    [[nodiscard]] static mid::Operand _evaluate_unary(const Type &to, auto expression);
};

} // namespace arkoi::opt