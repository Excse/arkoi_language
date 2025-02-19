#include "opt/simplify_cfg.hpp"

#include <cassert>

using namespace arkoi::opt;
using namespace arkoi;

bool SimplifyCFG::enter_function(il::Function &) {
    _mergable_blocks.clear();
    _proxy_blocks.clear();
    return false;
}

bool SimplifyCFG::on_block(il::BasicBlock &block) {
    if (_is_mergable_block(block)) {
        _mergable_blocks.insert(&block);
    } else if (_is_proxy_block(block)) {
        _proxy_blocks.insert(&block);
    }
    return false;
}

bool SimplifyCFG::exit_function(il::Function &function) {
    bool changed = false;

    for (auto &block: _proxy_blocks) {
        _remove_proxy_block(function, *block);
        changed = true;
    }

    for (auto &block: _mergable_blocks) {
        _merge_block(function, *block);
        changed = true;
    }

    return changed;
}

void SimplifyCFG::_remove_proxy_block(il::Function &function, il::BasicBlock &block) {
    auto &target = std::get<il::Goto>(block.instructions().front());
    auto *target_block = block.next();

    // Replace every predecessor goto with a copy of the current one. Also delete the predecessor, as a
    // empty predecessor vector is needed to delete a BasicBlock from a function.
    std::erase_if(block.predecessors(), [&](il::BasicBlock *predecessor) {
        auto &instruction = std::get<il::Goto>(predecessor->instructions().back());
        instruction = target;
        predecessor->set_next(target_block);
        return true;
    });

    // Be sure to delete the current block from the function.
    assert(function.remove(&block));
}

bool SimplifyCFG::_is_proxy_block(il::BasicBlock &block) {
    if (block.instructions().size() != 1) return false;
    if (!std::holds_alternative<il::Goto>(block.instructions().front())) return false;

    for (auto *predecessor: block.predecessors()) {
        if (predecessor->instructions().empty()) return false;

        auto *goto_instruction = std::get_if<il::Goto>(&predecessor->instructions().back());
        if (!goto_instruction) return false;
    }

    return true;
}

void SimplifyCFG::_merge_block(il::Function &function, il::BasicBlock &block) {
    auto *predecessor = *block.predecessors().begin();
    auto &instructions = predecessor->instructions();

    // Remove the goto instruction
    instructions.erase(instructions.end());

    // Move all the instructions from the current block to the predecessor
    instructions.insert(instructions.end(),
                        std::make_move_iterator(block.instructions().begin()),
                        std::make_move_iterator(block.instructions().end()));

    // Link the predecessor with the current next block
    auto *target_block = block.next();
    predecessor->set_next(target_block);

    // In case of merging a basic block with the exit block of a function, the exit block must be set to the
    // predecessor.
    if (&block == function.exit()) function.set_exit(predecessor);

    // Remove the predecessor, as it is required to remove the entire basic block from the function.
    block.predecessors().clear();

    // Be sure to delete the current block from the function.
    assert(function.remove(&block));
}

bool SimplifyCFG::_is_mergable_block(il::BasicBlock &block) {
    if (block.branch()) return false;
    if (block.predecessors().size() != 1) return false;

    auto *predecessor = *block.predecessors().begin();
    if (predecessor->instructions().empty()) return false;

    auto &instruction = predecessor->instructions().back();
    if (!std::holds_alternative<il::Goto>(instruction)) return false;

    return true;
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
