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

    // This will set _current_type
    node.return_type().accept(*this);
    _return_type = _current_type;

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
    node.expression()->accept(*this);
    auto type = _current_type;

    if (*type == *_return_type) {
        return;
    }

    if (!_can_implicit_convert(type, _return_type)) {
        throw std::runtime_error("Return statement has a wrong return type.");
    }

    // We assure to override the const casted node with a new node. Thus, this exception is legal.
    auto &expression = const_cast<std::unique_ptr<Node> &>(node.expression());
    node.set_expression(std::make_unique<CastNode>(std::move(expression), _to_typenode(type),
                                                   _to_typenode(_return_type)));
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

    auto result = _arithmetic_conversion(left, right);
    _current_type = result;

    if (*left != *result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &left_node = const_cast<std::unique_ptr<Node> &>(node.left());
        node.set_left(std::make_unique<CastNode>(std::move(left_node), _to_typenode(left), _to_typenode(result)));
    }

    if (*right != *result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &right_node = const_cast<std::unique_ptr<Node> &>(node.right());
        node.set_right(std::make_unique<CastNode>(std::move(right_node), _to_typenode(right), _to_typenode(result)));
    }
}

void TypeResolver::visit(CastNode &node) {
    std::shared_ptr<Type> from;
    if(!node.from()) {
        // This will set _current_type
        node.expression()->accept(*this);
        from = _current_type;

        node.set_from(_to_typenode(from));
    } else {
        // This will set _current_type
        node.from().value().accept(*this);
        from = _current_type;
    }

    // This will set _current_type
    node.to().accept(*this);
    auto to = _current_type;

    if(!_can_implicit_convert(from, to)) {
        throw std::runtime_error("This cast is not valid.");
    }

    auto result = _resolve_type(node.to());
    _current_type = result;
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
std::shared_ptr<Type> TypeResolver::_arithmetic_conversion(const std::shared_ptr<Type> &left_type,
                                                           const std::shared_ptr<Type> &right_type) {
    // TODO: 1. Add Stage 4 for floating-point numbers.

    // Stage 5: Both operands are converted to a common type C.
    auto t1 = std::dynamic_pointer_cast<IntegerType>(left_type);
    auto t2 = std::dynamic_pointer_cast<IntegerType>(right_type);
    if (!t1 || !t2) throw std::invalid_argument("This arithmetic conversion is not allowed.");

    // Given the types T1 and T2 as the promoted type (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1->size() < 32) t1 = std::make_shared<IntegerType>(32, t1->sign());
    if (t2->size() < 32) t2 = std::make_shared<IntegerType>(32, t2->sign());

    // 1. If T1 and T2 are the same type, C is that type.
    if (*t1 == *t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the type of greater integer
    //    conversion rank.
    if (t1->sign() == t2->sign()) {
        if (t2->size() > t1->size()) return t2;
        else return t1;
    }

    // 3. Otherwise, one type between T1 and T2 is an signed integer type S, the other type is an unsigned integer type U.
    //    Apply the following rules:
    auto _signed = t1->sign() ? t1 : t2;
    auto _unsigned = !t1->sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned->size() >= _signed->size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all of the values of U, C is S.
    if (_signed->max() >= _unsigned->max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer type corresponding to S.
    return std::make_shared<IntegerType>(_signed->size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const std::shared_ptr<Type> &from, const std::shared_ptr<Type> &destination) {
    // A prvalue of an integer type or of an unscoped enumeration type can be converted to any other integer type. If the
    // conversion is listed under integral promotions, it is a promotion and not a conversion.
    if (std::dynamic_pointer_cast<IntegerType>(destination) && std::dynamic_pointer_cast<IntegerType>(from)) {
        return true;
    }

    // TODO: Implement floating-point implicit conversion check

    return false;
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
