#include "type_resolver.h"

#include "utils.h"
#include "ast.h"

void TypeResolver::visit(const ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void TypeResolver::visit(const FunctionNode &node) {
    auto symbol = _scopes.top()->lookup<FunctionSymbol>(to_string(node.name().value()));

    _scopes.push(node.table());

    std::vector<std::shared_ptr<Type>> parameter_types;
    for (const auto &item: node.parameters()) {
        // This will set _current_type
        item.accept(*this);
        auto type = _current_type;

        parameter_types.push_back(type);
    }

    auto function = std::static_pointer_cast<FunctionSymbol>(symbol);
    function->set_parameter_types(std::move(parameter_types));

    node.block().accept(*this);
    _scopes.pop();
}

void TypeResolver::visit(const BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void TypeResolver::visit(const ParameterNode &node) {
    // This will set _current_type
    node.type().accept(*this);
    auto type = _current_type;

    auto symbol = _scopes.top()->lookup<ParameterSymbol>(to_string(node.name().value()));
    auto parameter = std::static_pointer_cast<ParameterSymbol>(symbol);

    parameter->set_type(type);
}

void TypeResolver::visit(const TypeNode &node) {
    _current_type = _resolve_type(node);
}

void TypeResolver::visit(const NumberNode &node) {
    auto value = std::stoull(to_string(node.value().value()));
    _current_type = std::make_shared<IntegerType>(value);
}

void TypeResolver::visit(const ReturnNode &node) {
    node.expression().accept(*this);
}

void TypeResolver::visit(const IdentifierNode &node) {
    auto symbol = _scopes.top()->lookup<ParameterSymbol>(to_string(node.value().value()));
    if (auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(symbol)) {
        _current_type = parameter->type();
    }
}

void TypeResolver::visit(const BinaryNode &node) {
    // This will set _current_type
    node.left().accept(*this);
    auto left = _current_type;

    // This will set _current_type
    node.right().accept(*this);
    auto right = _current_type;

    auto result = _maximum_type(left, right);
    _current_type = result;
}

std::shared_ptr<Type> TypeResolver::_maximum_type(const std::shared_ptr<Type> &first,
                                                  const std::shared_ptr<Type> &second) {
    if (first == second) return first;

    if (first->can_implicitly_cast_to(*second)) return second;
    if (second->can_implicitly_cast_to(*first)) return first;

    throw std::runtime_error("Cant decide a maximum type for those types.");
}

std::shared_ptr<Type> TypeResolver::_resolve_type(const TypeNode &node) {
    switch (node.token().type()) {
        case Token::Type::U8: return std::make_shared<IntegerType>(8, false);
        case Token::Type::S8: return std::make_shared<IntegerType>(8, true);
        case Token::Type::U16: return std::make_shared<IntegerType>(16, false);
        case Token::Type::S16: return std::make_shared<IntegerType>(16, true);
        case Token::Type::U32: return std::make_shared<IntegerType>(32, false);
        case Token::Type::S32: return std::make_shared<IntegerType>(32, true);
        case Token::Type::U64: return std::make_shared<IntegerType>(64, false);
        case Token::Type::S64: return std::make_shared<IntegerType>(64, true);
        case Token::Type::USize: return std::make_shared<IntegerType>(64, false);
        case Token::Type::SSize: return std::make_shared<IntegerType>(64, true);
        default: throw std::runtime_error("Can't resolve this type.");
    }
}
