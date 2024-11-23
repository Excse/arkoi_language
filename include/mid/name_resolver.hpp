#pragma once

#include <stack>

#include "mid/symbol_table.hpp"
#include "utils/visitor.hpp"
#include "front/token.hpp"

namespace arkoi::mid {

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

    void visit(ast::Integer &) override {};

    void visit(ast::Floating &) override {};

    void visit(ast::Boolean &) override {};

    void visit(ast::Return &node) override;

    void visit(ast::Binary &node) override;

    void visit(ast::Cast &node) override;

    void visit(ast::Call &node) override;

    void visit(ast::If &node) override;

    template<typename Type, typename... Args>
    [[nodiscard]] SharedSymbol _check_non_existence(const front::Token &token, Args &&... args);

    template<typename... Types>
    [[nodiscard]] SharedSymbol _check_existence(const front::Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../../src/mid/name_resolver.tpp"

}