#pragma once

#include <stack>

#include "semantic/type.hpp"
#include "frontend/ast.hpp"
#include "utils/visitor.hpp"

class TypeResolver : node::Visitor {
private:
    TypeResolver() = default;

public:
    [[nodiscard]] static TypeResolver resolve(node::Program &node);

    void visit(node::Program &node) override;

    void visit_as_prototype(node::Function &node);

    void visit(node::Function &node) override;

    void visit(node::Block &node) override;

    void visit(node::Parameter &node) override;

    void visit(node::Integer &node) override;

    void visit(node::Floating &node) override;

    void visit(node::Boolean &node) override;

    void visit(node::Return &node) override;

    void visit(node::Identifier &node) override;

    void visit(node::Binary &node) override;

    void visit(node::Cast &node) override;

    void visit(node::Call &node) override;

    void visit(node::If &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static Type _arithmetic_conversion(const Type &left_type, const Type &right_type);

    static bool _can_implicit_convert(const Type &from, const Type &destination);

private:
    std::optional<Type> _current_type{}, _return_type{};
    bool _failed{};
};
