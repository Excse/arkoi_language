#include "il/cfg.hpp"

void BasicBlock::depth_first_search(const std::function<void(BasicBlock &)> &callback,
                                    std::unordered_set<BasicBlock *> &visited) {
    if (visited.contains(&*this)) return;
    visited.insert(&*this);

    callback(*this);

    if (_branch) _branch->depth_first_search(callback, visited);
    if (_next) _next->depth_first_search(callback, visited);
}

void Function::depth_first_search(const std::function<void(BasicBlock &)> &callback) {
    std::unordered_set<BasicBlock *> visited;

    // We manually callback the end basic block as it should always be the last one being invoked.
    visited.insert(_end.get());

    _start->depth_first_search(callback, visited);

    callback(*_end);
}

void Function::linearize(const std::function<void(Instruction &)> &callback) {
    auto visit_instructions = [&](BasicBlock &block) {
        for (auto &instruction: block.instructions()) {
            callback(*instruction);
        }
    };

    depth_first_search(visit_instructions);
}
