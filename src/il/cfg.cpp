#include "il/cfg.hpp"

#include <unordered_set>
#include <stack>

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

        if (current->next()) queue.push(current->next().get());
        if (current->branch()) queue.push(current->branch().get());
    }

    callback(*_end);
}

void Function::linearize(const std::function<void(il::InstructionType &)> &callback) {
    auto visit_instructions = [&](BasicBlock &block) {
        for (auto &instruction: block.instructions()) {
            callback(instruction);
        }
    };

    depth_first_search(visit_instructions);
}
