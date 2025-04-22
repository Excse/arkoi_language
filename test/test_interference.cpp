#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "utils/interference_graph.hpp"

using testing::UnorderedElementsAre;

TEST(InterferenceGraphTest, AddNodesAndEdges) {
    InterferenceGraph<std::string> graph;

    graph.add_node("a");
    graph.add_node("b");
    graph.add_node("c");

    graph.add_edge("a", "b");
    graph.add_edge("a", "c");

    EXPECT_TRUE(graph.is_interfering("a", "b"));
    EXPECT_TRUE(graph.is_interfering("a", "c"));
    EXPECT_FALSE(graph.is_interfering("b", "c"));

    const auto interferences = graph.interferences("a");
    EXPECT_THAT(interferences, UnorderedElementsAre("b", "c"));
}

TEST(InterferenceGraphTest, DuplicateAdditionsDontCrash) {
    InterferenceGraph<std::string> graph;

    graph.add_node("x");
    graph.add_node("x");

    graph.add_edge("x", "y");
    graph.add_edge("x", "y");

    EXPECT_TRUE(graph.is_interfering("x", "y"));
    EXPECT_FALSE(graph.is_interfering("x", "z"));
}

TEST(InterferenceGraphTest, IsolatedNodeHasNoNeighbors) {
    InterferenceGraph<std::string> graph;

    graph.add_node("u");

    const auto interferences = graph.interferences("u");
    EXPECT_TRUE(interferences.empty());
}

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
