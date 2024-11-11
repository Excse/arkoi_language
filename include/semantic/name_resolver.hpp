#pragma once

#include <stack>

#include "semantic/symbol_table.hpp"
#include "frontend/token.hpp"
#include "utils/visitor.hpp"

class NameResolver : node::Visitor {
private:
    NameResolver() = default;

public:
    [[nodiscard]] static NameResolver resolve(node::Program &node);

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    void visit(node::Program &node) override;

    void visit_as_prototype(node::Function &node);

    void visit(node::Function &node) override;

    void visit(node::Block &node) override;

    void visit(node::Parameter &) override;

    void visit(node::Identifier &node) override;

    void visit(node::Integer &) override {};

    void visit(node::Floating &) override {};

    void visit(node::Boolean &) override {};

    void visit(node::Return &node) override;

    void visit(node::Binary &node) override;

    void visit(node::Cast &node) override;

    void visit(node::Call &node) override;

    void visit(node::If &node) override;

    template<typename Type, typename... Args>
    [[nodiscard]] SharedSymbol _check_non_existence(const Token &token, Args &&... args);

    template<typename... Types>
    [[nodiscard]] SharedSymbol _check_existence(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../../src/semantic/name_resolver.tpp"
