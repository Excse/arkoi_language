#pragma once

#include <stack>

#include "symbol_table.hpp"
#include "utils/visitor.hpp"
#include "frontend/token.hpp"

class NameResolver : NodeVisitor {
private:
    NameResolver() = default;

public:
    [[nodiscard]] static NameResolver resolve(ProgramNode &node);

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    void visit(ProgramNode &node) override;

    void visit_as_prototype(FunctionNode &node);

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override;

    void visit(IdentifierNode &node) override;

    void visit(IntegerNode &) override {};

    void visit(FloatingNode &) override {};

    void visit(BooleanNode &) override {};

    void visit(ReturnNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    void visit(CallNode &node) override;

    void visit(IfNode &node) override;

    template<typename SymbolType, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const Token &token, Args &&... args);

    template<typename... SymbolTypes>
    [[nodiscard]] std::shared_ptr<Symbol> _check_existence(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../../src/semantic/name_resolver.tpp"