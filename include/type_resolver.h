#pragma once

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"
#include "type.h"
#include "ast.h"

class TypeResolver : public NodeVisitor {
public:
    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &node) override;

    void visit(IntegerNode &node) override;

    void visit(FloatingNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static Type _arithmetic_conversion(const Type &left_type, const Type &right_type);

    static bool _can_implicit_convert(const Type &from, const Type &destination);

private:
    Type _current_type{std::monostate()};
    Type _return_type{std::monostate()};
    size_t _sse_index{}, _int_index{};
    bool _failed{};
};