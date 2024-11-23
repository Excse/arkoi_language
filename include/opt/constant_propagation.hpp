#pragma once

#include "opt/optimization.hpp"

namespace arkoi::opt {

class ConstantPropagation : public Pass {
public:
    using Constants = std::unordered_map<mid::Variable, mid::Constant>;

public:
    bool new_function(mid::Function &) override;

    bool new_block(mid::BasicBlock &block) override;

private:
    void _add_constant(mid::InstructionType &type);

    bool _can_propagate(mid::InstructionType &type);

    bool _propagate(mid::Operand &operand);

private:
    Constants _constants{};
};

}