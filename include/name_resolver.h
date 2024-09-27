#pragma once

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"

class NameResolver : NodeVisitor {
private:
    NameResolver() = default;

public:
    [[nodiscard]] static NameResolver resolve(ProgramNode &node);

    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &) override;

    void visit(IdentifierNode &node) override;

    void visit(IntegerNode &) override {};

    void visit(FloatingNode &) override {};

    void visit(ReturnNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    void visit(CallNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

    template<typename SymbolType, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const Token &token, Args &&... args);

    template<typename... SymbolTypes>
    [[nodiscard]] std::shared_ptr<Symbol> _check_existence(const Token &token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    bool _failed{};
};

#include "../src/name_resolver.tpp"