#pragma once

#include "optimization.hpp"

class ConstantPropagation : public Pass {
public:
    using Constants = std::unordered_map<il::Variable, il::Constant>;

public:
    bool new_function(Function &) override;

    bool new_block(BasicBlock &block) override;

private:
    void _add_constant(il::InstructionType &type);

    bool _can_propagate(il::InstructionType &type);

    bool _propagate(il::Operand &operand);

private:
    Constants _constants{};
};
