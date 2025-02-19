#include "opt/simplify_cfg.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool SimplifyCFG::enter_function(il::Function &) {
    _proxy_blocks.clear();
    return false;
}

bool SimplifyCFG::on_block(il::BasicBlock &block) {
    // TODO: Testzwecke
    if(block.predecessors().empty()) return false;

    // TODO: Fix this!! Instead of storing raw pointers, the actual smartpointer should be stored. As it could
    //       corrupt the memory, when deleting the link between BasicBlocks.
    if (_is_proxy_block(block)) {
        _proxy_blocks.insert(&block);
    }

    return false;
}

bool SimplifyCFG::exit_function(il::Function &) {
    bool changed = false;

    for (auto &block: _proxy_blocks) changed |= _remove_proxy_block(*block);

    return changed;
}

bool SimplifyCFG::_is_proxy_block(il::BasicBlock &block) {
    if (block.instructions().size() != 1) return false;
    return std::holds_alternative<il::Goto>(block.instructions().front());
}

bool SimplifyCFG::_remove_proxy_block(il::BasicBlock &block) {
    auto target_goto = std::get<il::Goto>(block.instructions().front());
    auto target_label = target_goto.label();
    auto target_next = block.next();

    for (auto *predecessor: block.predecessors()) {
        if (predecessor->instructions().empty()) return false;

        auto *goto_instruction = std::get_if<il::Goto>(&predecessor->instructions().back());
        if (!goto_instruction) return false;

        goto_instruction->set_label(target_label);
//        predecessor->set_next(target_next);
    }

    block.predecessors().clear();

//    block.remove_next();

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
