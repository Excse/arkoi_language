#include "x86_64/memory_mapping.hpp"

#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

MemoryMapper MemoryMapper::map(il::Function &function) {
    MemoryMapper mapper;
    mapper.visit(function);
    return mapper;
}

void MemoryMapper::visit(il::Function &function) {
    for (auto &block: function) {
        block.accept(*this);
    }
}

void MemoryMapper::visit(il::BasicBlock &block) {
    for (auto &instruction: block) {
        instruction.accept(*this);
    }
}

void MemoryMapper::visit(il::Binary &instruction) {
    _add_mapping(instruction.result(), instruction.result_type().size());
}

void MemoryMapper::visit(il::Cast &instruction) {
    _add_mapping(instruction.result(), instruction.to().size());
}

void MemoryMapper::visit(il::Call &instruction) {
    _add_mapping(instruction.result(), instruction.type().size());
}

void MemoryMapper::visit(il::Alloca &instruction) {
    _add_mapping(instruction.result(), instruction.type().size());
}

void MemoryMapper::visit(il::Load &instruction) {
    _add_mapping(instruction.result(), instruction.type().size());
}

void MemoryMapper::visit(il::Constant &instruction) {
    _add_mapping(instruction.result(), instruction.type().size());
}

MemoryMapper::StackLocation &MemoryMapper::operator[](il::Variable variable) {
    if(!_mappings.contains(variable)) {
        throw std::invalid_argument("This variable is not mapped.");
    }
    return _mappings.at(variable);
}

void MemoryMapper::_add_mapping(const il::Variable &variable, Size size) {
    _mappings[variable] = _stack_size;
    _stack_size += size_to_bytes(size);
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
