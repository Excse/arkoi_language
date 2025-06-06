#include <cassert>

template<DataflowPassConcept Pass>
void DataflowAnalysis<Pass>::run(Function &function) {
    _out.clear();
    _in.clear();

    std::stack<BasicBlock *> worklist;

    for (auto &block: function) {
        if constexpr (Pass::Granularity == DataflowGranularity::Block) {
            if constexpr (Pass::Direction == DataflowDirection::Forward) {
                _out[&block] = _pass->initialize(function, block);
            } else {
                _in[&block] = _pass->initialize(function, block);
            }
        } else {
            for (auto &instruction: block) {
                if constexpr (Pass::Direction == DataflowDirection::Forward) {
                    _out[&instruction] = _pass->initialize(function, instruction);
                } else {
                    _in[&instruction] = _pass->initialize(function, instruction);
                }
            }
        }

        worklist.push(&block);
    }

    while (!worklist.empty()) {
        auto *block = worklist.top();
        worklist.pop();

        // A requirement for every basic block.
        assert(!block->instructions().empty());

        if constexpr (Pass::Granularity == DataflowGranularity::Block) {
            auto &old_out = _out[block];
            auto &old_in = _in[block];

            if constexpr (Pass::Direction == DataflowDirection::Forward) {
                std::vector<State> states;
                for (auto *predecessor: block->predecessors()) {
                    states.push_back(_out[predecessor]);
                }

                auto new_in = _pass->merge(states);
                auto new_out = _pass->transfer(*block, new_in);
                old_in = std::move(new_in);

                if (new_out == old_out) continue;
                old_out = std::move(new_out);

                if (block->next()) worklist.push(block->next());
                if (block->branch()) worklist.push(block->branch());
            } else {
                std::vector<State> states;
                if (block->next()) states.push_back(_in[block->next()]);
                if (block->branch()) states.push_back(_in[block->branch()]);

                auto new_out = _pass->merge(states);
                auto new_in = _pass->transfer(*block, new_out);
                old_out = std::move(new_out);

                if (new_in == old_in) continue;
                old_in = std::move(new_in);

                for (auto *predecessor: block->predecessors()) {
                    worklist.push(predecessor);
                }
            }
        } else {
            if constexpr (Pass::Direction == DataflowDirection::Forward) {
                std::vector<State> states;
                for (auto *predecessor: block->predecessors()) {
                    if (predecessor->instructions().empty()) continue;

                    const auto &last_instruction = predecessor->instructions().back();
                    states.push_back(_out[&last_instruction]);
                }

                auto &first_instruction = block->instructions().front();
                auto new_in = _pass->merge(states);
                auto new_out = _pass->transfer(first_instruction, new_in);
                _in[&first_instruction] = std::move(new_in);

                auto changed = new_out != _out[&first_instruction];
                _out[&first_instruction] = std::move(new_out);

                for (auto it = block->instructions().begin() + 1; it != block->instructions().end(); ++it) {
                    auto &previous_instruction = *(it - 1);
                    auto &instruction = *it;

                    std::vector<State> states;
                    states.push_back(_out[&previous_instruction]);

                    auto new_in = _pass->merge(states);
                    auto new_out = _pass->transfer(instruction, new_in);
                    _in[&instruction] = std::move(new_in);

                    changed |= new_out != _out[&instruction];
                    _out[&instruction] = std::move(new_out);
                }

                if (!changed) continue;

                for (auto *predecessor: block->predecessors()) {
                    worklist.push(predecessor);
                }
            } else {
                std::vector<State> states;
                if (block->next()) {
                    assert(!block->next()->instructions().empty());

                    auto &first_instruction = block->next()->instructions().front();
                    states.push_back(_in[&first_instruction]);
                }
                if (block->branch()) {
                    assert(!block->branch()->instructions().empty());

                    auto &first_instruction = block->branch()->instructions().front();
                    states.push_back(_in[&first_instruction]);
                }

                auto &last_instruction = block->instructions().back();
                auto new_out = _pass->merge(states);
                auto new_in = _pass->transfer(last_instruction, new_out);
                _out[&last_instruction] = std::move(new_out);

                auto changed = new_in != _in[&last_instruction];
                _in[&last_instruction] = std::move(new_in);

                for (auto it = block->instructions().rbegin() + 1; it != block->instructions().rend(); ++it) {
                    auto &previous_instruction = *(it - 1);
                    auto &instruction = *it;

                    std::vector<State> states;
                    states.push_back(_in[&previous_instruction]);

                    auto new_out = _pass->merge(states);
                    auto new_in = _pass->transfer(instruction, new_out);
                    _out[&instruction] = std::move(new_out);

                    changed |= new_in != _in[&instruction];
                    _in[&instruction] = std::move(new_in);
                }

                if (!changed) continue;

                for (auto *predecessor: block->predecessors()) {
                    worklist.push(predecessor);
                }
            }
        }
    }
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
