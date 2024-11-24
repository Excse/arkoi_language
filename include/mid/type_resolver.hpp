#pragma once

#include <stack>

#include "utils/visitor.hpp"
#include "def/ast.hpp"
#include "def/type.hpp"

namespace arkoi::mid {

class TypeResolver : ast::Visitor {
private:
    TypeResolver() = default;

public:
    [[nodiscard]] static TypeResolver resolve(ast::Program &node);

    void visit(ast::Program &node) override;

    void visit_as_prototype(ast::Function &node);

    void visit(ast::Function &node) override;

    void visit(ast::Block &node) override;

    void visit(ast::Parameter &node) override;

    void visit(ast::Integer &node) override;

    void visit(ast::Floating &node) override;

    void visit(ast::Boolean &node) override;

    void visit(ast::Return &node) override;

    void visit(ast::Identifier &node) override;

    void visit(ast::Binary &node) override;

    void visit(ast::Cast &node) override;

    void visit(ast::Call &node) override;

    void visit(ast::If &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static Type _arithmetic_conversion(const Type &left_type, const Type &right_type);

    static bool _can_implicit_convert(const Type &from, const Type &destination);

private:
    std::optional<Type> _current_type{}, _return_type{};
    bool _failed{};
};

} // namespace arkoi::mid