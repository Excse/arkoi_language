#pragma once

#include <stack>

#include "semantic/symbol_table.hpp"
#include "utils/visitor.hpp"
#include "frontend/token.hpp"
#include "semantic/type.hpp"
#include "frontend/ast.hpp"

namespace arkoi {

class TypeResolver : ast::NodeVisitor {
private:
    TypeResolver() = default;

public:
    [[nodiscard]] static TypeResolver resolve(ast::ProgramNode &node);

    void visit(ast::ProgramNode &node) override;

    void visit_as_prototype(ast::FunctionNode &node);

    void visit(ast::FunctionNode &node) override;

    void visit(ast::BlockNode &node) override;

    void visit(ast::ParameterNode &node) override;

    void visit(ast::IntegerNode &node) override;

    void visit(ast::FloatingNode &node) override;

    void visit(ast::BooleanNode &node) override;

    void visit(ast::ReturnNode &node) override;

    void visit(ast::IdentifierNode &node) override;

    void visit(ast::BinaryNode &node) override;

    void visit(ast::CastNode &node) override;

    void visit(ast::CallNode &node) override;

    void visit(ast::IfNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static type::Type _arithmetic_conversion(const type::Type &left_type, const type::Type &right_type);

    static bool _can_implicit_convert(const type::Type &from, const type::Type &destination);

private:
    std::optional<type::Type> _current_type{}, _return_type{};
    size_t _sse_index{}, _int_index{};
    bool _failed{};
};

}