#include "il/cfg.hpp"

#include <cassert>
#include <stack>

using namespace arkoi::il;

void BasicBlock::set_branch(BasicBlock *branch) {
    if(branch) branch->_predecessors.insert(this);
    _branch = branch;
}

void BasicBlock::set_next(BasicBlock *next) {
    if(next) next->_predecessors.insert(this);
    _next = next;
}

BlockIterator::BlockIterator(Function *function)
    : _visited(), _queue(), _function(function), _current(nullptr) {
    if (!function) return;

    auto *start = function->entry();

    _visited.insert(function->exit());
    _queue.push(start);

    ++(*this);
}

BlockIterator &BlockIterator::operator++() {
    if (_current == _function->exit()) {
        _current = nullptr;
        return *this;
    }

    if (_queue.empty()) {
        _current = _function->exit();
        return *this;
    }

    _current = _queue.top();
    _queue.pop();
    _visited.insert(_current);

    if (_current->branch() && !_visited.contains(_current->branch())) {
        _queue.push(_current->branch());
    }

    if (_current->next() && !_visited.contains(_current->next())) {
        _queue.push(_current->next());
    }

    return *this;
}

BlockIterator BlockIterator::operator++(int) {
    BlockIterator temp = *this;
    std::ignore = ++(*this);
    return temp;
}

Function::Function(std::string name, std::vector<Variable> parameters, Type type)
    : _block_pool(), _parameters(std::move(parameters)), _name(std::move(name)), _type(std::move(type)) {
    _entry = emplace_back(_name + "_entry");
    _exit = emplace_back(_name + "_exit");
}

bool Function::remove(BasicBlock *target) {
    assert(target->predecessors().empty());

    if (target->next()) target->next()->predecessors().erase(target);
    if (target->branch()) target->branch()->predecessors().erase(target);

    return std::erase_if(_block_pool, [&](const std::shared_ptr<BasicBlock> &block) {
        return block.get() == target;
    });
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
