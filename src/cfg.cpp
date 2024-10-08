#include "cfg.hpp"

void BasicBlock::depth_first_search(const std::function<void(const BasicBlock &)> &callback,
                                    std::unordered_set<const BasicBlock *> &visited) const {
    if (visited.contains(&*this)) return;
    visited.insert(&*this);

    callback(*this);

    if (_next) _next->depth_first_search(callback, visited);
    if (_branch) _branch->depth_first_search(callback, visited);
}

void Function::depth_first_search(const std::function<void(const BasicBlock &)> &callback) const {
    std::unordered_set<const BasicBlock *> visited;

    // We manually callback the end basic block as it should always be the last one being invoked.
    visited.insert(_end.get());

    _start->depth_first_search(callback, visited);

    callback(*_end);
}
