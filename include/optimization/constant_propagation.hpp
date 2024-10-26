#pragma once

#include "optimization.hpp"

class ConstantPropagation : public Pass {
public:
    using Constants = std::unordered_map<il::Variable, il::Constant>;

public:
    bool new_function(Function &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    static void _propagate_if(il::If &instruction, const Constants &constants);

    static void _propagate_call(il::Call &instruction, const Constants &constants);

    static void _propagate_return(il::Return &instruction, const Constants &constants);

    static void _propagate_binary(il::Binary &instruction, const Constants &constants);

    static void _propagate_cast(il::Cast &instruction, const Constants &constants);
};
