#pragma once

#include <stack>

#include "symbol_table.hpp"
#include "utils/visitor.hpp"
#include "frontend/token.hpp"

namespace arkoi {

class NameResolver : ast::NodeVisitor {
private:
    NameResolver() = default;

public:
    [[nodiscard]] static NameResolver resolve(ast::ProgramNode &node);

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    void visit(ast::ProgramNode &node) override;

    void visit_as_prototype(ast::FunctionNode &node);

    void visit(ast::FunctionNode &node) override;

    void visit(ast::BlockNode &node) override;

    void visit(ast::ParameterNode &) override;

    void visit(ast::IdentifierNode &node) override;

    void visit(ast::IntegerNode &) override {};

    void visit(ast::FloatingNode &) override {};

    void visit(ast::BooleanNode &) override {};

    void visit(ast::ReturnNode &node) override;

    void visit(ast::BinaryNode &node) override;

    void visit(ast::CastNode &node) override;

    void visit(ast::CallNode &node) override;

    void visit(ast::IfNode &node) override;

    template<typename SymbolType, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const Token &token, Args &&... args);

    template<typename... SymbolTypes>
    [[nodiscard]] std::shared_ptr<Symbol> _check_existence(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../../src/semantic/name_resolver.tpp"

}