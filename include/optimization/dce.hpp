#pragma once

#include <set>

#include "optimization.hpp"

class DeadCodeElimination : public Pass {
public:
    using UsedVariables = std::set<const il::Variable *>;

public:
    bool new_function(Function &) override;

    bool new_block(BasicBlock &block) override;

private:
    void _mark_as_used(const il::Operand &operand);

    bool _eliminate_dead_stores(BasicBlock &block);

private:
    UsedVariables _used{};
};
