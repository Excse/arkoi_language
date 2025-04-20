#pragma once

#include "il/cfg.hpp"
#include "il/instruction.hpp"
#include "sem/type.hpp"

namespace arkoi::il {

class Generator final : ast::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Module generate(ast::Program &node);

    void visit(ast::Program &node) override;

    void visit(ast::Function &node) override;

    void visit(ast::Block &node) override;

    void visit(ast::Parameter &) override {}

    void visit(ast::Immediate &node) override;

    void visit_integer(ast::Immediate &node);

    void visit_floating(ast::Immediate &node);

    void visit_boolean(ast::Immediate &node);

    void visit(ast::Return &node) override;

    void visit(ast::Identifier &node) override;

    void visit(ast::Binary &node) override;

    void visit(ast::Cast &node) override;

    void visit(ast::Assign &node) override;

    void visit(ast::Call &node) override;

    void visit(ast::If &node) override;

    [[nodiscard]] auto &module() { return _module; }

private:
    std::string _make_label_symbol();

    Variable _make_temporary(const sem::Type &type);

    Memory _make_memory(const sem::Type &type);

private:
    std::unordered_map<std::shared_ptr<Symbol>, Memory> _allocas{};
    std::optional<Memory> _return_temp{};
    size_t _temp_index{}, _label_index{};
    Function *_current_function{};
    BasicBlock *_current_block{};
    Operand _current_operand{};
    Module _module;
};

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
