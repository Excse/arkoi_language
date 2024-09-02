#ifndef ARKOI_LANGUAGE_TYPE_RESOLVER_H
#define ARKOI_LANGUAGE_TYPE_RESOLVER_H

#include <stack>

#include "symbol_table.h"
#include "visitor.h"
#include "token.h"
#include "type.h"

class TypeResolver : public NodeVisitor {
public:
    TypeResolver() : _scopes(), _current_type(), _failed(false) {}

    void visit(const ProgramNode &node) override;

    void visit(const FunctionNode &node) override;

    void visit(const TypeNode &node) override;

    void visit(const BlockNode &node) override;

    void visit(const ParameterNode &node) override;

    void visit(const NumberNode &node) override;

    void visit(const ReturnNode &node) override;

    void visit(const IdentifierNode &node) override;

    void visit(const BinaryNode &node) override;

    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    static std::shared_ptr<Type> _maximum_type(const std::shared_ptr<Type> &first, const std::shared_ptr<Type> &second);

    static std::shared_ptr<Type> _resolve_type(const TypeNode &node);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{};
    std::shared_ptr<Type> _current_type;
    bool _failed;
};


#endif //ARKOI_LANGUAGE_TYPE_RESOLVER_H
