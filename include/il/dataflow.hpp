#pragma once

#include <concepts>
#include <set>

#include "il/instruction.hpp"
#include "il/cfg.hpp"

namespace arkoi::il {

template<typename Result>
using State = std::set<Result>;

template<typename Result>
class DataflowPass {
    virtual ~DataflowPass() = 0;

    virtual std::set<Result> initialize_entry(Function &function, BasicBlock &entry) = 0;

    virtual std::set<Result> initialize(BasicBlock &current) = 0;

    virtual std::set<Result> merge(std::vector<State<Result> &> &predecessors) = 0;

    virtual std::set<Result> transfer(BasicBlock &current, State<Result> &state) = 0;

    virtual constexpr bool is_forward() = 0;
};

template<typename Result>
class DataflowAnalysis {
public:
    DataflowAnalysis(DataflowPass<Result> pass) : _pass(std::move(pass)) {}

    void run(Function &function);

private:
    std::unordered_map<BasicBlock *, State<Result>> _out{};
    std::unordered_map<BasicBlock *, State<Result>> _in{};
    DataflowPass<Result> _pass;
};

template<typename Result>
void DataflowAnalysis<Result>::run(Function &function) {
    std::stack<BasicBlock *> worklist;

    if constexpr (_pass.is_forward()) {
        _in[function.entry()] = _pass.initialize_entry(function, *function.entry());
    } else {
        _out[function.exit()] = _pass.initialize_entry(function, *function.exit());
    }

    for (auto &block: function) {
        if constexpr (_pass.is_forward()) {
            _out[&block] = _pass.initialize(block);
        } else {
            _in[&block] = _pass.initialize(block);
        }

        worklist.push(&block);
    }

    while (!worklist.empty()) {
        auto *block = worklist.top();
        worklist.pop();

        auto &old_out = _out[block];
        auto &old_in = _in[block];

        std::vector<State<Result> &> states;
        if constexpr (_pass.is_forward()) {
            for (auto *predecessor: block->predecessors()) {
                states.push_back(_out[predecessor]);
            }

            auto new_in = _pass.merge(states);
            auto new_out = _pass.transfer(*block, new_in);
            old_in = std::move(new_in);

            if (new_out == old_out) continue;
            old_out = std::move(new_out);

            if (block->next()) worklist.push(block->next());
            if (block->branch()) worklist.push(block->branch());
        } else {
            if (block->next()) states.push_back(_in[block->next()]);
            if (block->branch()) states.push_back(_in[block->branch()]);

            auto new_out = _pass.merge(states);
            auto new_in = _pass.transfer(*block, new_out);
            old_out = std::move(new_out);

            if (new_in == old_in) continue;
            old_in = std::move(new_in);

            for (auto *predecessor: block->predecessors()) {
                worklist.push(predecessor);
            }
        }
    }
}

} // namespace arkoi::il

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
