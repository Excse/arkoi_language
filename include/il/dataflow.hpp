#pragma once

#include <concepts>
#include <set>

#include "il/instruction.hpp"
#include "il/cfg.hpp"

namespace arkoi::il {

enum class DataflowDirection {
    Forward,
    Backward
};

template<typename Result>
using State = std::set<Result>;

template<typename Result>
class DataflowPass {
public:
    using ResultType [[maybe_unused]] = Result;

public:
    virtual ~DataflowPass() = default;

    virtual std::set<Result> initialize_entry(Function &function, BasicBlock &entry) = 0;

    virtual std::set<Result> initialize(BasicBlock &current) = 0;

    virtual std::set<Result> merge(const std::vector<State<Result>> &predecessors) = 0;

    virtual std::set<Result> transfer(BasicBlock &current, State<Result> &state) = 0;
};

template<typename Pass>
class DataflowAnalysis {
    static_assert(std::is_base_of<DataflowPass<typename Pass::ResultType>, Pass>::value,
        "Pass must be a subclass of DataflowPass");
    static_assert(std::is_same<decltype(Pass::Direction), const DataflowDirection>::value,
        "Derived class must define a static constexpr DataflowDirection Direction");

public:
    template<typename... Args>
    DataflowAnalysis(Args&&... args) : _pass(std::make_unique<Pass>(std::forward<Args>(args)...)) {}

    void run(Function &function);

    [[nodiscard]] auto &out() { return _out; }

    [[nodiscard]] auto &in() { return _in; }

private:
    std::unordered_map<BasicBlock *, State<typename Pass::ResultType>> _out{};
    std::unordered_map<BasicBlock *, State<typename Pass::ResultType>> _in{};
    std::unique_ptr<Pass> _pass;
};

#include "../../src/il/dataflow.tpp"

} // namespace arkoi::il

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
