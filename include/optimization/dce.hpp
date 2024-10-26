#pragma once

#include <set>

#include "optimization.hpp"

class DeadCodeElimination : public Pass {
public:
    using UsedVariables = std::set<const il::Variable *>;

public:
    bool new_function(Function &) override { return false; }

    bool new_block(BasicBlock &block) override;

private:
    static void _mark_as_used(UsedVariables &used, const il::Operand &operand);

    static bool _eliminate_dead_stores(BasicBlock &block);
};
