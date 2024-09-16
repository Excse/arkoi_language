#include "type_resolver.h"

#include "utils.h"
#include "ast.h"

void TypeResolver::visit(ProgramNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit(FunctionNode &node) {
    // Reset the register counters for the parameters
    _sse_index = 0;
    _int_index = 0;

    std::vector<std::shared_ptr<Type>> parameter_types;
    for (auto &item: node.parameters()) {
        // This will set _current_type
        item.accept(*this);
        auto type = _current_type;

        parameter_types.push_back(type);
    }

    auto function = std::static_pointer_cast<FunctionSymbol>(node.symbol());
    function->set_parameter_types(std::move(parameter_types));

    _return_type = node.type();

    node.block().accept(*this);
}

void TypeResolver::visit(BlockNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit(ParameterNode &node) {
    auto parameter = std::static_pointer_cast<ParameterSymbol>(node.symbol());
    parameter->set_type(node.type());

    if (std::dynamic_pointer_cast<IntegerType>(node.type())) {
        parameter->set_int_index(_int_index++);
    } else if (std::dynamic_pointer_cast<FloatingType>(node.type())) {
        parameter->set_sse_index(_sse_index++);
    }
}

void TypeResolver::visit(IntegerNode &node) {
    auto number_string = node.value().contents();
    auto sign = !number_string.starts_with('-');

    int64_t size;
    if (sign) {
        size = std::stoll(number_string) > std::numeric_limits<int32_t>::max() ? 64 : 32;
    } else {
        size = std::stoull(number_string) > std::numeric_limits<uint32_t>::max() ? 64 : 32;
    }

    _current_type = std::make_shared<IntegerType>(size, sign);
}

void TypeResolver::visit(FloatingNode &node) {
    auto number_string = node.value().contents();

    auto size = std::stold(number_string) > std::numeric_limits<float>::max() ? 64 : 32;
    _current_type = std::make_shared<FloatingType>(size);
}

void TypeResolver::visit(ReturnNode &node) {
    node.expression()->accept(*this);
    auto type = _current_type;

    node.set_type(_return_type);

    if (*type == *_return_type) {
        return;
    }

    if (!_can_implicit_convert(type, _return_type)) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    // We assure to override the const casted node with a new node. Thus, this exception is legal.
    auto &expression = const_cast<std::unique_ptr<Node> &>(node.expression());
    node.set_expression(std::make_unique<CastNode>(std::move(expression), type, _return_type));
}

void TypeResolver::visit(IdentifierNode &node) {
    auto parameter = std::static_pointer_cast<ParameterSymbol>(node.symbol());
    _current_type = parameter->type();
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
    node.set_type(result);

    if (*left != *result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &left_node = const_cast<std::unique_ptr<Node> &>(node.left());
        node.set_left(std::make_unique<CastNode>(std::move(left_node), left, result));
    }

    if (*right != *result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &right_node = const_cast<std::unique_ptr<Node> &>(node.right());
        node.set_right(std::make_unique<CastNode>(std::move(right_node), right, result));
    }
}

void TypeResolver::visit(CastNode &node) {
    // This will set _current_type
    node.expression()->accept(*this);
    auto from = _current_type;
    node.set_from(from);

    if (!_can_implicit_convert(from, node.to())) {
        throw std::runtime_error("This cast is not valid.");
    }

    _current_type = node.to();
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
std::shared_ptr<Type> TypeResolver::_arithmetic_conversion(const std::shared_ptr<Type> &left_type,
                                                           const std::shared_ptr<Type> &right_type) {
    auto floating_left = std::dynamic_pointer_cast<FloatingType>(left_type);
    auto floating_right = std::dynamic_pointer_cast<FloatingType>(right_type);

    // Stage 4: If either operand is of floating-point type, the following rules are applied:
    if (floating_left || floating_right) {
        // If both operands have the same type, no further conversion will be performed.
        if (*left_type == *right_type) return left_type;

        // Otherwise, if one of the operands is of a non-floating-point type, that operand is converted to the type of
        // the other operand.
        if (floating_left && !floating_right) return floating_left;
        if (floating_right && !floating_left) return floating_right;

        // Otherwise, if the floating-point conversion ranks of the types of the operands are ordered but(since C++23)
        // not equal, then the operand of the type with the lesser floating-point conversion rank is converted to the
        // type of the other operand.
        if (floating_left && floating_right) {
            if (floating_left->size() > floating_right->size()) return floating_left;
            if (floating_right->size() > floating_left->size()) return floating_right;
        }
    }

    // Otherwise, both operands are of integer types, proceed to the next stage.
    auto integer_left = std::dynamic_pointer_cast<IntegerType>(left_type);
    auto integer_right = std::dynamic_pointer_cast<IntegerType>(right_type);
    if (!integer_left || !integer_right) throw std::invalid_argument("This arithmetic conversion is not allowed.");

    // Stage 5: Both operands are converted to a common op C.
    auto t1 = integer_left;
    auto t2 = integer_right;

    // Given the types T1 and T2 as the promoted op (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1->size() < 32) t1 = std::make_shared<IntegerType>(32, t1->sign());
    if (t2->size() < 32) t2 = std::make_shared<IntegerType>(32, t2->sign());

    // 1. If T1 and T2 are the same type, C is that op.
    if (*t1 == *t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the op of greater integer
    //    conversion rank.
    if (t1->sign() == t2->sign()) {
        if (t2->size() > t1->size()) return t2;
        else return t1;
    }

    // 3. Otherwise, one type between T1 and T2 is an signed integer type S, the other type is an unsigned integer op U.
    //    Apply the following rules:
    auto _signed = t1->sign() ? t1 : t2;
    auto _unsigned = !t1->sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned->size() >= _signed->size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all of the values of U, C is S.
    if (_signed->max() >= _unsigned->max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer op corresponding to S.
    return std::make_shared<IntegerType>(_signed->size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const std::shared_ptr<Type> &from, const std::shared_ptr<Type> &destination) {
    // A prvalue of an integer type or of an unscoped enumeration op can be converted to any other integer type. If the
    // conversion is listed under integral promotions, it is a promotion and not a conversion.
    if (std::dynamic_pointer_cast<IntegerType>(from) && std::dynamic_pointer_cast<IntegerType>(destination)) {
        return true;
    }

    // A prvalue of a floating-point type can be converted to a prvalue of any other floating-point type. (until C++23)
    if (std::dynamic_pointer_cast<FloatingType>(from) && std::dynamic_pointer_cast<FloatingType>(destination)) {
        return true;
    }

    // A prvalue of floating-point type can be converted to a prvalue of any integer type. The fractional part is
    // truncated, that is, the fractional part is discarded.
    if (std::dynamic_pointer_cast<FloatingType>(from) && std::dynamic_pointer_cast<IntegerType>(destination)) {
        return true;
    }

    // A prvalue of integer or unscoped enumeration type can be converted to a prvalue of any floating-point type.
    // The result is exact if possible.
    if (std::dynamic_pointer_cast<IntegerType>(from) && std::dynamic_pointer_cast<FloatingType>(destination)) {
        return true;
    }

    return false;
}
