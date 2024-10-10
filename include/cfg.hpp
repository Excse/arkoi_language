#pragma once

#include <vector>
#include <memory>

#include "instruction.hpp"

struct BasicBlock {
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::shared_ptr<BasicBlock> next, branch;
};