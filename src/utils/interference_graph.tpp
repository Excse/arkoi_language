template<typename Node>
void InterferenceGraph<Node>::add_node(const Node &node) {
    _adjacents.emplace(node, std::unordered_set<Node>());
}

template<typename Node>
void InterferenceGraph<Node>::remove_node(const Node &node) {
    auto found = _adjacents.find(node);
    if (found == _adjacents.end()) return;

    for (const auto &adjacent: found->second) {
        _adjacents[adjacent].erase(node);
    }

    _adjacents.erase(found);
}

template<typename Node>
void InterferenceGraph<Node>::add_edge(const Node &first, const Node &second) {
    if (first == second) return;

    add_node(first);
    add_node(second);
    _adjacents[first].insert(second);
    _adjacents[second].insert(first);
}

template<typename Node>
bool InterferenceGraph<Node>::is_interfering(const Node &first, const Node &second) const {
    auto found = _adjacents.find(first);
    if (found == _adjacents.end()) return false;
    return found->second.count(second) > 0;
}

template<typename Node>
std::unordered_set<Node> InterferenceGraph<Node>::interferences(const Node &node) const {
    auto found = _adjacents.find(node);
    if (found == _adjacents.end()) return {};
    return found->second;
}

template<typename Node>
std::unordered_set<Node> InterferenceGraph<Node>::nodes() const {
    std::unordered_set<Node> nodes;
    for (const auto &[node, _]: _adjacents) nodes.insert(node);
    return nodes;
}

template<typename Node>
std::ostream &operator<<(std::ostream &os, const InterferenceGraph<Node> &graph) {
    os << "graph InterferenceGraph {\n";

    for (const auto &[node, _]: graph.adjacents()) os << "    \"\\" << node << "\"\n";

    for (const auto &[node, adjacents]: graph.adjacents()) {
        for (const auto &adjacent: adjacents) {
            if (node < adjacent) continue;

            os << "    \"\\" << node << "\" -- \"\\" << adjacent << "\"\n";
        }
    }

    os << "}\n";

    return os;
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
