#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "il/cfg.hpp"

using testing::ElementsAre;
using namespace arkoi;

/**
 * main:
 *                  [   entry   ]
 *                 /            \
 *          [next_1]             [branch_1]
 *         /       \                 |
 * [next_2]         [branch_2]       |
 *        \               |      /--/
 *         \-------\      |     /
 *                  [   exit   ]
 */
il::Function create_example_cfg() {
    auto entry_block = std::make_shared<il::BasicBlock>("entry");
    auto next_1_block = std::make_shared<il::BasicBlock>("next_1");
    auto next_2_block = std::make_shared<il::BasicBlock>("next_2");
    auto branch_2_block = std::make_shared<il::BasicBlock>("branch_2");
    auto branch_1_block = std::make_shared<il::BasicBlock>("branch_1");
    auto exit_block = std::make_shared<il::BasicBlock>("exit");

    auto function_symbol = std::make_shared<Symbol>(sem::Function("main"));
    il::Function function(function_symbol, entry_block, exit_block);

    entry_block->set_next(next_1_block);
    entry_block->set_branch(branch_1_block);

    next_1_block->set_next(next_2_block);
    next_1_block->set_branch(branch_2_block);

    branch_1_block->set_next(exit_block);

    next_2_block->set_next(exit_block);
    branch_2_block->set_next(exit_block);

    return function;
}

TEST(ControlFlowGraph, IteratorRightOrder) {
    il::Function function = create_example_cfg();

    std::vector<std::string> labels;
    std::transform(function.begin(), function.end(),
                   std::back_inserter(labels),
                   [](const il::BasicBlock &block) {
                       return block.label();
                   });

    ASSERT_EQ(labels.size(), 6);
    EXPECT_THAT(labels, ElementsAre("entry", "next_1", "next_2", "branch_2", "branch_1", "exit"));
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