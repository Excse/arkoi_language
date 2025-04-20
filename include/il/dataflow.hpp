#pragma once

#include <concepts>

#include "il/cfg.hpp"
#include "il/instruction.hpp"

namespace arkoi::il {

enum class DataflowDirection {
    Forward,
    Backward
};

enum class DataflowGranularity {
    Block,
    Instruction
};

template<typename ResultType, DataflowDirection DirectionType, DataflowGranularity GranularityType>
class DataflowPass {
public:
    using Result [[maybe_unused]] = ResultType;
    using State = std::unordered_set<Result>;

    static constexpr DataflowGranularity Granularity [[maybe_unused]] = GranularityType;
    static constexpr DataflowDirection Direction [[maybe_unused]] = DirectionType;

public:
    virtual ~DataflowPass() = default;

    virtual State merge(const std::vector<State> &predecessors) = 0;

    virtual State initialize(Function &, BasicBlock &) {
        if constexpr (Granularity != DataflowGranularity::Block) return State{};
        throw std::runtime_error("initialize(BasicBlock &) must be implemented for block-level analysis");
    }

    virtual State transfer(BasicBlock &, const State &state) {
        if constexpr (Granularity != DataflowGranularity::Block) return state;
        throw std::runtime_error("transfer(BasicBlock &, State &) must be implemented for block-level analysis");
    }

    virtual State initialize(Function &, Instruction &) {
        if constexpr (Granularity != DataflowGranularity::Instruction) return State{};
        throw std::runtime_error("initialize(Instruction &) must be implemented for instruction-level analysis");
    }

    virtual State transfer(Instruction &, const State &state) {
        if constexpr (Granularity != DataflowGranularity::Instruction) return state;
        throw std::runtime_error("transfer(Instruction &, State &) must be implemented for instruction-level analysis");
    }
};

template <typename T>
concept DataflowPassConcept = requires {
    typename T::Result;
    { T::Direction } -> std::convertible_to<DataflowDirection>;
    { T::Granularity } -> std::convertible_to<DataflowGranularity>;
} && std::is_base_of_v<DataflowPass<typename T::Result, T::Direction, T::Granularity>, T>;

template<DataflowPassConcept Pass>
class DataflowAnalysis {
public:
    using Key = std::conditional_t<Pass::Granularity == DataflowGranularity::Block, BasicBlock *, Instruction *>;
    using State = std::unordered_set<typename Pass::Result>;

public:
    template<typename... Args>
    explicit DataflowAnalysis(Args&&... args) : _pass(std::make_unique<Pass>(std::forward<Args>(args)...)) {}

    void run(Function &function);

    [[nodiscard]] auto &out() const { return _out; }

    [[nodiscard]] auto &in() const { return _in; }

private:
    std::unordered_map<Key, State> _out{};
    std::unordered_map<Key, State> _in{};
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
