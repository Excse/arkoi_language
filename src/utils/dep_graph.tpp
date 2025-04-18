#include <queue>
#include <stack>
#include <stdexcept>
#include <unordered_set>

template <typename ID>
void DependencyGraph<ID>::add_node(ID node) {
    _graph.try_emplace(node);
}

template <typename ID>
void DependencyGraph<ID>::add_dependency(ID node, ID dependency) {
    add_node(node);
    add_node(dependency);

    if (_has_path(node, dependency)) {
        throw std::runtime_error("Cannot add this dependency as it would result in a cycle.");
    }

    _graph[dependency].push_back(node);
}

template <typename ID>
std::vector<ID> DependencyGraph<ID>::topological_sort() const {
    std::unordered_map<ID, size_t> in_degree;
    for (const auto &[node, neighbors]: _graph) {
        in_degree.try_emplace(node);

        for (const auto &neighbor: neighbors) {
            ++in_degree[neighbor];
        }
    }

    std::queue<ID> queue;
    for (const auto &[node, degree]: in_degree) {
        if (degree == 0) queue.push(node);
    }

    std::vector<ID> ordered;
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        ordered.push_back(node);

        for (const auto &neighbor: _graph.at(node)) {
            auto degree = --in_degree[neighbor];
            if (degree == 0) queue.push(neighbor);
        }
    }

    if(ordered.size() != _graph.size()) {
        throw std::runtime_error("A cycle has been detected. A topological sort is not possible.");
    }

    return ordered;
}

template <typename ID>
bool DependencyGraph<ID>::_has_path(ID start, ID target) {
    std::unordered_set<ID> visited;
    std::stack<ID> queue;

    queue.push(start);

    while (!queue.empty()) {
        auto current = queue.top();
        queue.pop();

        if (current == target) return true;

        if (visited.contains(current)) continue;

        visited.insert(current);

        for (const auto &neighbor: _graph[current]) {
            if (visited.contains(neighbor)) continue;

            queue.push(neighbor);
        }
    }

    return false;
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
