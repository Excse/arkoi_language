#include "type_resolver.h"

#include "utils.h"
#include "ast.h"

void TypeResolver::visit(ProgramNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void TypeResolver::visit(FunctionNode &node) {
    auto symbol = _scopes.top()->lookup<FunctionSymbol>(to_string(node.name().value()));

    _scopes.push(node.table());

    std::vector<std::shared_ptr<Type>> parameter_types;
    for (auto &item: node.parameters()) {
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

void TypeResolver::visit(BlockNode &node) {
    _scopes.push(node.table());
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
    _scopes.pop();
}

void TypeResolver::visit(ParameterNode &node) {
    // This will set _current_type
    node.type().accept(*this);
    auto type = _current_type;

    auto symbol = _scopes.top()->lookup<ParameterSymbol>(to_string(node.name().value()));
    auto parameter = std::static_pointer_cast<ParameterSymbol>(symbol);

    parameter->set_type(type);
}

void TypeResolver::visit(TypeNode &node) {
    _current_type = _resolve_type(node);
}

void TypeResolver::visit(NumberNode &node) {
    auto value = std::stoull(to_string(node.value().value()));
    _current_type = std::make_shared<IntegerType>(value);
}

void TypeResolver::visit(ReturnNode &node) {
    node.expression().accept(*this);
}

void TypeResolver::visit(IdentifierNode &node) {
    auto symbol = _scopes.top()->lookup<ParameterSymbol>(to_string(node.value().value()));
    if (auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(symbol)) {
        _current_type = parameter->type();
    }
}

void TypeResolver::visit(BinaryNode &node) {
    // This will set _current_type
    node.left()->accept(*this);
    auto left = _current_type;

    // This will set _current_type
    node.right()->accept(*this);
    auto right = _current_type;

    auto result = _arithmetic_conversion(node.left(), left, node.right(), right);
    _current_type = result;
}

void TypeResolver::visit(CastNode &node) {
    // This will set _current_type
    node.expression()->accept(*this);
    auto from = _current_type;
    node.set_from(_to_typenode(from));

    // TODO: Do check if this casting is even allowed.

    auto result = _resolve_type(node.to());
    _current_type = result;
}

void TypeResolver::_integer_promote(std::shared_ptr<IntegerType> &type, std::unique_ptr<Node> &node) {
    if (type->size() >= 32) return;

    if (type->sign()) {
        node = std::make_unique<CastNode>(std::move(node), _to_typenode(type), TypeNode::TYPE_S32);
    } else {
        node = std::make_unique<CastNode>(std::move(node), _to_typenode(type), TypeNode::TYPE_U32);
    }

    type = std::make_shared<IntegerType>(32, type->sign());
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
std::shared_ptr<Type> TypeResolver::_arithmetic_conversion(
        std::unique_ptr<Node> &left_node, std::shared_ptr<Type> &left_type,
        std::unique_ptr<Node> &right_node, std::shared_ptr<Type> &right_type) {
    // TODO: 1. Add Stage 4 for floating-point numbers.

    // Stage 5
    // Both operands are converted to a common type C.
    auto t1 = std::dynamic_pointer_cast<IntegerType>(left_type);
    auto t2 = std::dynamic_pointer_cast<IntegerType>(right_type);
    if (!t1 || !t2) throw std::invalid_argument("This arithmetic conversion is not allowed.");

    // Given the types T1 and T2 as the promoted type (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    _integer_promote(t1, left_node);
    _integer_promote(t2, right_node);

    // 1. If T1 and T2 are the same type, C is that type.
    if (*t1 == *t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the type of greater integer
    //    conversion rank.
    if (t1->sign() == t2->sign()) {
        if (t2->size() > t1->size()) return _cast_node(left_node, t1, t2);
        if (t1->size() > t2->size()) return _cast_node(right_node, t2, t1);
    }

    // 3. Otherwise, one type between T1 and T2 is an signed integer type S, the other type is an unsigned integer type U.
    //    Apply the following rules:
    if (t1->sign()) {
        // 3.1. If the integer conversion rank of T2 is greater than or equal to the integer conversion rank of T1, C is T2.
        if (t2->size() >= t1->size()) return _cast_node(left_node, t1, t2);

        // 3.2. Otherwise, if T1 can represent all of the values of T2, C is T1.
        if (t1->max() >= t2->max()) return _cast_node(right_node, t2, t1);

        // 3.3. Otherwise, C is the unsigned integer type corresponding to T1.
        auto result = std::make_shared<IntegerType>(t1->size(), false);
        _cast_node(left_node, t1, result);
        _cast_node(right_node, t2, result);
        return result;
    } else {
        // 3.1. If the integer conversion rank of T1 is greater than or equal to the integer conversion rank of T2, C is T1.
        if (t1->size() >= t2->size()) return _cast_node(right_node, t2, t1);

        // 3.2. Otherwise, if T2 can represent all of the values of T1, C is T2.
        if (t2->max() >= t1->max()) return _cast_node(left_node, t1, t2);

        // 3.3. Otherwise, C is the unsigned integer type corresponding to T2.
        auto result = std::make_shared<IntegerType>(t2->size(), false);
        _cast_node(left_node, t1, result);
        _cast_node(right_node, t2, result);
        return result;
    }
}

std::shared_ptr<IntegerType> TypeResolver::_cast_node(std::unique_ptr<Node> &node,
                                                      const std::shared_ptr<IntegerType> &from,
                                                      const std::shared_ptr<IntegerType> &to) {
    node = std::make_unique<CastNode>(std::move(node), _to_typenode(from), _to_typenode(to));
    return to;
}

std::shared_ptr<Type> TypeResolver::_resolve_type(const TypeNode &node) {
    switch (node.token().type()) {
        case Token::Type::U8: return IntegerType::TYPE_U8;
        case Token::Type::S8: return IntegerType::TYPE_S8;
        case Token::Type::U16: return IntegerType::TYPE_U16;
        case Token::Type::S16: return IntegerType::TYPE_S16;
        case Token::Type::U32: return IntegerType::TYPE_U32;
        case Token::Type::S32: return IntegerType::TYPE_S32;
        case Token::Type::U64: return IntegerType::TYPE_U64;
        case Token::Type::S64: return IntegerType::TYPE_S64;
        case Token::Type::USize: return IntegerType::TYPE_USize;
        case Token::Type::SSize: return IntegerType::TYPE_SSize;
        default: throw std::invalid_argument("Can't resolve this type.");
    }
}

TypeNode TypeResolver::_to_typenode(const std::shared_ptr<Type> &type) {
    if (auto integer = std::dynamic_pointer_cast<IntegerType>(type)) {
        if (integer->sign()) {
            switch (integer->size()) {
                case 8: return TypeNode::TYPE_S8;
                case 16: return TypeNode::TYPE_S16;
                case 32: return TypeNode::TYPE_S32;
                case 64: return TypeNode::TYPE_S64;
            }
        } else {
            switch (integer->size()) {
                case 8: return TypeNode::TYPE_U8;
                case 16: return TypeNode::TYPE_U16;
                case 32: return TypeNode::TYPE_U32;
                case 64: return TypeNode::TYPE_U64;
            }
        }
    }

    throw std::invalid_argument("This type is not supported.");
}
