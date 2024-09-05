#ifndef ARKOI_LANGUAGE_TYPE_RESOLVER_H
#define ARKOI_LANGUAGE_TYPE_RESOLVER_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"
#include "type.h"
#include "ast.h"

class TypeResolver : public NodeVisitor {
public:
    TypeResolver() : _scopes(), _current_type(), _return_type(), _failed(false) {}

    void visit(ProgramNode &node) override;

    void visit(FunctionNode &node) override;

    void visit(TypeNode &node) override;

    void visit(BlockNode &node) override;

    void visit(ParameterNode &node) override;

    void visit(NumberNode &node) override;

    void visit(ReturnNode &node) override;

    void visit(IdentifierNode &node) override;

    void visit(BinaryNode &node) override;

    void visit(CastNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

    static std::shared_ptr<Type> _resolve_type(const TypeNode &node);

    static TypeNode _to_typenode(const std::shared_ptr<Type> &type);

private:
    static std::shared_ptr<Type> _arithmetic_conversion(const std::shared_ptr<Type> &left_type,
                                                        const std::shared_ptr<Type> &right_type);

    static bool _can_implicit_convert(const std::shared_ptr<Type> &from, const std::shared_ptr<Type> &destination);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::shared_ptr<Type> _current_type;
    std::shared_ptr<Type> _return_type;
    bool _failed;
};


#endif //ARKOI_LANGUAGE_TYPE_RESOLVER_H
