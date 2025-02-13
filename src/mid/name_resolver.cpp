#include "mid/name_resolver.hpp"

#include "utils/utils.hpp"
#include "def/ast.hpp"

using namespace arkoi::mid;

NameResolver NameResolver::resolve(ast::Program &node) {
    NameResolver resolver;

    node.accept(resolver);

    return resolver;
}

void NameResolver::visit(ast::Program &node) {
    _scopes.push(node.table());

    // At first all function prototypes are name resolved.
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<ast::Function *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }

    _scopes.pop();
}

void NameResolver::visit_as_prototype(ast::Function &node) {
    auto symbol = _check_non_existence<symbol::Function>(node.name());
    node.set_symbol(symbol);
}

void NameResolver::visit(ast::Function &node) {
    _scopes.push(node.table());

    std::vector<SharedSymbol> parameters;
    for (auto &item: node.parameters()) {
        item.accept(*this);

        parameters.push_back(item.symbol());
    }

    auto &function = std::get<symbol::Function>(*node.symbol());
    function.set_parameters(std::move(parameters));

    node.block()->accept(*this);
    _scopes.pop();
}

void NameResolver::visit(ast::Parameter &node) {
    auto parameter_symbol = _check_non_existence<symbol::Parameter>(node.name());
    node.set_symbol(parameter_symbol);
}

void NameResolver::visit(ast::Block &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void NameResolver::visit(ast::Identifier &node) {
    auto symbol = _check_existence<symbol::Parameter>(node.value());
    node.set_symbol(symbol);
}

void NameResolver::visit(ast::Return &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(ast::Binary &node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void NameResolver::visit(ast::Cast &node) {
    node.expression()->accept(*this);
}

void NameResolver::visit(ast::If &node) {
    node.condition()->accept(*this);

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (node.branch()) std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
}

void NameResolver::visit(ast::Assign &node) {
    auto symbol = _check_existence<symbol::Parameter>(node.name());
    node.set_symbol(symbol);

    node.expression()->accept(*this);
}

void NameResolver::visit(ast::Call &node) {
    auto symbol = _check_existence<symbol::Function>(node.name());
    node.set_symbol(symbol);

    for (const auto &item: node.arguments()) {
        item->accept(*this);
    }
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
