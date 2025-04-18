#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "utils/dep_graph.hpp"

using testing::ElementsAre;

TEST(DependencyGraph, NoDependencies) {
    DependencyGraph<size_t> graph;

    for (size_t index = 0; index < 10; index++) {
        graph.add_node(index);
    }

    const auto order = graph.topological_sort();
    ASSERT_EQ(order.size(), 10);
    EXPECT_THAT(order, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST(DependencyGraph, Dependencies) {
    DependencyGraph<size_t> graph;

    for (size_t index = 0; index < 10; index++) {
        graph.add_node(index);

        if (index != 0) {
            graph.add_dependency(index - 1, index);
        }
    }

    const auto order = graph.topological_sort();
    ASSERT_EQ(order.size(), 10);
    EXPECT_THAT(order, ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
}

TEST(DependencyGraph, Cycle) {
    DependencyGraph<size_t> graph;

    graph.add_dependency(1, 2);
    ASSERT_THROW(graph.add_dependency(2, 1), std::runtime_error);
}

/**
 *       A
 *       |
 *       v
 *       B ----> C <---- D
 *       |       |
 *       v       v
 *       E <---- F <---- G
 *       |
 *       v
 *       H
 */
TEST(DependencyGraph, Complex) {
    DependencyGraph<size_t> graph;

    graph.add_dependency('B', 'A');
    graph.add_dependency('C', 'B');
    graph.add_dependency('C', 'D');
    graph.add_dependency('E', 'B');
    graph.add_dependency('E', 'F');
    graph.add_dependency('F', 'G');
    graph.add_dependency('F', 'C');
    graph.add_dependency('H', 'E');

    const auto order = graph.topological_sort();
    ASSERT_EQ(order.size(), 8);
    EXPECT_THAT(order, ElementsAre('A', 'D', 'G', 'B', 'C', 'F', 'E', 'H'));
}

/**
 *       A
 *       |
 *       v
 *       B ----> C <---- D
 *       |       |
 *       v       v
 *       E <---- F <---- G
 *       |
 *       v
 *       H
  */
TEST(DependencyGraph, ComplexEnum) {
    enum Node : size_t {
        A = 10, B = 1, C = 3, D = 5, E = 20, F = 2, G = 11, H = 12
    };

    DependencyGraph<Node> graph;

    graph.add_dependency(B, A);
    graph.add_dependency(C, B);
    graph.add_dependency(C, D);
    graph.add_dependency(E, B);
    graph.add_dependency(E, F);
    graph.add_dependency(F, G);
    graph.add_dependency(F, C);
    graph.add_dependency(H, E);

    const auto order = graph.topological_sort();
    ASSERT_EQ(order.size(), 8);
    EXPECT_THAT(order, ElementsAre(A, D, G, B, C, F, E, H));
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