#pragma once

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"
#include "type.h"
#include "ast.h"

class TypeResolver : NodeVisitor {
private:
    TypeResolver() = default;

public:
    [[nodiscard]] static TypeResolver resolve(ProgramNode &node);

    void visit(ProgramNode &node) override;

    void visit_as_prototype(FunctionNode &node);

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &node) override;

    void visit(IntegerNode &node) override;

    void visit(FloatingNode &node) override;

    void visit(BooleanNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    void visit(CallNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static Type _arithmetic_conversion(const Type &left_type, const Type &right_type);

    static bool _can_implicit_convert(const Type &from, const Type &destination);

private:
    Type _current_type{}, _return_type{};
    size_t _sse_index{}, _int_index{};
    bool _failed{};
};