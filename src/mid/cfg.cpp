#include "mid/cfg.hpp"

#include <unordered_set>
#include <stack>

using namespace arkoi::mid;

void Function::depth_first_search(const std::function<void(BasicBlock &)> &callback) {
    std::unordered_set<BasicBlock *> visited;
    std::stack<BasicBlock *> queue;

    // We manually callback the end basic block as it should always be the last one being invoked.
    visited.insert(_end.get());
    // Start with the entry basic block.
    queue.push(_start.get());

    while (!queue.empty()) {
        BasicBlock *current = queue.top();
        queue.pop();

        if (visited.contains(current)) continue;

        visited.insert(current);

        callback(*current);

        if (current->branch()) queue.push(current->branch().get());
        if (current->next()) queue.push(current->next().get());
    }

    callback(*_end);
}

void Function::linearize(const std::function<void(mid::InstructionType &)> &callback) {
    auto visit_instructions = [&](BasicBlock &block) {
        for (auto &instruction: block.instructions()) {
            callback(instruction);
        }
    };

    depth_first_search(visit_instructions);
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
