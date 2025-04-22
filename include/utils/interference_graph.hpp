#pragma once

#include <unordered_map>
#include <unordered_set>
#include <ostream>

template<typename Node>
class InterferenceGraph {
public:
    InterferenceGraph() = default;

    /**
     * Adds a node to the interference graph.
     *
     * If the node does not already exist in the graph, it is added
     * with no adjacent (connected) nodes. If the node already exists,
     * this function has no effect.
     *
     * @param node The node to be added to the interference graph.
     */
    void add_node(const Node &node);

    /**
     * Removes a node and all associated edges from the interference graph.
     *
     * If the specified node exists in the graph, this method will remove
     * the node and update the adjacency sets of all other nodes that were
     * connected to it. If the node does not exist, the method does nothing.
     *
     * @param node The node to remove from the interference graph.
     */
    void remove_node(const Node &node);

    /**
     * Adds an edge between two nodes in the graph.
     * If the nodes do not already exist in the graph, they will be added automatically.
     * If the two nodes are the same, no action is taken.
     *
     * The method ensures that the two nodes become adjacent to each other,
     * meaning an undirected edge is created between the provided nodes.
     *
     * @param first The first node of the edge.
     * @param second The second node of the edge.
     */
    void add_edge(const Node &first, const Node &second);

    /**
     * Checks if two nodes in the interference graph are interfering with each other.
     *
     * This method determines whether there is an interference (edge) between the
     * given nodes `first` and `second` in the graph. Interference is defined as
     * the presence of a direct connection between these nodes within the adjacency
     * structure of the graph.
     *
     * @param first The first node to check for interference.
     * @param second The second node to check for interference.
     * @return True if `first` and `second` are interfering (connected by an edge),
     *         false otherwise.
     */
    [[nodiscard]] bool is_interfering(const Node &first, const Node &second) const;

    /**
     * Retrieves the set of nodes that interfere with the given node in the graph.
     *
     * @param node The node for which to retrieve the interferences.
     * @return An unordered_set containing the nodes that are adjacent (interfere)
     *         with the specified node. If the node is not found in the graph,
     *         returns an empty unordered_set.
     */
    [[nodiscard]] std::unordered_set<Node> interferences(const Node &node) const;

    /**
     * Retrieves all nodes within the interference graph.
     *
     * @return A vector containing all nodes currently stored in the graph.
     */
    [[nodiscard]] std::unordered_set<Node> nodes() const;

    [[nodiscard]] auto &adjacents() const { return _adjacents; }

private:
    std::unordered_map<Node, std::unordered_set<Node>> _adjacents;
};

template<typename Node>
std::ostream &operator<<(std::ostream &os, const InterferenceGraph<Node> &graph);

#include "../../src/utils/interference_graph.tpp"

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
