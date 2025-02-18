#pragma once

#include <stack>

#include "sem/symbol_table.hpp"
#include "front/token.hpp"
#include "ast/visitor.hpp"

namespace arkoi::sem {

class NameResolver : ast::Visitor {
private:
    NameResolver() = default;

public:
    [[nodiscard]] static NameResolver resolve(ast::Program &node);

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    void visit(ast::Program &node) override;

    void visit_as_prototype(ast::Function &node);

    void visit(ast::Function &node) override;

    void visit(ast::Block &node) override;

    void visit(ast::Parameter &) override;

    void visit(ast::Identifier &node) override;

    void visit(ast::Immediate &) override {};

    void visit(ast::Return &node) override;

    void visit(ast::Binary &node) override;

    void visit(ast::Cast &node) override;

    void visit(ast::Assign &node) override;

    void visit(ast::Call &node) override;

    void visit(ast::If &node) override;

    template<typename Type, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const front::Token &token, Args &&... args);

    template<typename... Types>
    [[nodiscard]] std::shared_ptr<Symbol> _check_existence(const front::Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../../src/sem/name_resolver.tpp"

} // namespace arkoi::mid

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
