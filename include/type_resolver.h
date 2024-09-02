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
    TypeResolver() : _scopes(), _current_type(), _failed(false) {}

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

private:
    static std::shared_ptr<Type> _arithmetic_conversion(
            std::unique_ptr<Node> &left_node, std::shared_ptr<Type> &left_type,
            std::unique_ptr<Node> &right_node, std::shared_ptr<Type> &right_type);

    static void _integer_promote(std::shared_ptr<IntegerType> &type, std::unique_ptr<Node> &node);

    static std::shared_ptr<IntegerType> _cast_node(std::unique_ptr<Node> &node, const std::shared_ptr<IntegerType> &from,
                                                   const std::shared_ptr<IntegerType> &to);

    static std::shared_ptr<Type> _resolve_type(const TypeNode &node);

    static TypeNode _to_typenode(const std::shared_ptr<Type> &type);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::shared_ptr<Type> _current_type;
    bool _failed;
};


#endif //ARKOI_LANGUAGE_TYPE_RESOLVER_H
