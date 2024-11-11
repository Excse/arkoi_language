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
    void _mark_instruction(const il::InstructionType &type);

    void _mark_variable(const il::Operand &operand);

    bool _is_dead_store(const il::InstructionType &type);

    bool _eliminate_dead_stores(BasicBlock &block);

private:
    UsedVariables _used_variables{};
};
