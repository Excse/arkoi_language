#pragma once

#include <set>

#include "opt/optimization.hpp"

namespace arkoi::opt {

class DeadCodeElimination : public Pass {
public:
    using UsedVariables = std::set<const mid::Variable *>;

public:
    bool new_function(mid::Function &) override;

    bool new_block(mid::BasicBlock &block) override;

private:
    void _mark_instruction(const mid::InstructionType &type);

    void _mark_variable(const mid::Operand &operand);

    bool _is_dead_store(const mid::InstructionType &type);

    bool _eliminate_dead_stores(mid::BasicBlock &block);

private:
    UsedVariables _used_variables{};
};

} // namespace arkoi::opt
