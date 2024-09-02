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
    std::shared_ptr<Type> _arithmetic_conversion(std::unique_ptr<Node> &left, std::unique_ptr<Node> &right);

    static void _integer_promote(std::shared_ptr<IntegerType> &type, std::unique_ptr<Node> &node);

    static std::shared_ptr<Type> _resolve_type(const TypeNode &node);

    static TypeNode _to_typenode(const IntegerType &type);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::shared_ptr<Type> _current_type;
    bool _failed;
};


#endif //ARKOI_LANGUAGE_TYPE_RESOLVER_H
