#include "semantic/type_resolver.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "frontend/ast.hpp"

using namespace arkoi::type;
using namespace arkoi::ast;
using namespace arkoi;

static IntegralType BOOL_PROMOTED_INT_TYPE(Size::DWORD, false);
static BooleanType BOOL_TYPE;

TypeResolver TypeResolver::resolve(ProgramNode &node) {
    TypeResolver resolver;

    node.accept(resolver);

    return resolver;
}

void TypeResolver::visit(ProgramNode &node) {
    // At first all function prototypes are type resolved.
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<FunctionNode *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit_as_prototype(FunctionNode &node) {
    // Reset the register counters for the parameters
    _sse_index = 0;
    _int_index = 0;

    for (auto &item: node.parameters()) {
        item.accept(*this);
    }

    auto &function = std::get<FunctionSymbol>(*node.symbol());
    function.set_return_type(node.type());
}

void TypeResolver::visit(FunctionNode &node) {
    _return_type = node.type();

    node.block()->accept(*this);
}

void TypeResolver::visit(BlockNode &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit(ParameterNode &node) {
    auto &parameter = std::get<ParameterSymbol>(*node.symbol());
    parameter.set_type(node.type());

    std::visit(match {
        [&](const IntegralType &) { parameter.set_int_index(_int_index++); },
        [&](const BooleanType &) { parameter.set_int_index(_int_index++); },
        [&](const FloatingType &) { parameter.set_sse_index(_sse_index++); }
    }, node.type());
}

void TypeResolver::visit(IntegerNode &node) {
    const auto &number_string = node.value().contents();
    auto sign = !number_string.starts_with('-');

    Size size;
    if (sign) {
        size = std::stoll(number_string) > std::numeric_limits<int32_t>::max() ? Size::QWORD : Size::DWORD;
    } else {
        size = std::stoull(number_string) > std::numeric_limits<uint32_t>::max() ? Size::QWORD : Size::DWORD;
    }

    _current_type = IntegralType(size, sign);
}

void TypeResolver::visit(FloatingNode &node) {
    const auto &number_string = node.value().contents();

    auto size = std::stold(number_string) > std::numeric_limits<float>::max() ? Size::QWORD : Size::DWORD;
    _current_type = FloatingType(size);
}

void TypeResolver::visit(BooleanNode &) {
    _current_type = BooleanType();
}

void TypeResolver::visit(ReturnNode &node) {
    node.expression()->accept(*this);
    auto type = _current_type.value();

    node.set_type(_return_type.value());

    if (type == _return_type.value()) {
        return;
    }

    if (!_can_implicit_convert(type, _return_type.value())) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    // We assure to override the const casted node with a new node. Thus, this exception is legal.
    auto &expression = const_cast<std::unique_ptr<Node> &>(node.expression());
    node.set_expression(std::make_unique<CastNode>(std::move(expression), type, _return_type.value()));
}

void TypeResolver::visit(IdentifierNode &node) {
    const auto &parameter = std::get<ParameterSymbol>(*node.symbol());
    _current_type = parameter.type();
}

void TypeResolver::visit(BinaryNode &node) {
    // This will set _current_type
    node.left()->accept(*this);
    auto left = _current_type.value();

    // This will set _current_type
    node.right()->accept(*this);
    auto right = _current_type.value();

    auto result = _arithmetic_conversion(left, right);
    _current_type = result;
    node.set_type(result);

    if (left != result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &left_node = const_cast<std::unique_ptr<Node> &>(node.left());
        node.set_left(std::make_unique<CastNode>(std::move(left_node), left, result));
    }

    if (right != result) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &right_node = const_cast<std::unique_ptr<Node> &>(node.right());
        node.set_right(std::make_unique<CastNode>(std::move(right_node), right, result));
    }
}

void TypeResolver::visit(CastNode &node) {
    // This will set _current_type
    node.expression()->accept(*this);
    auto from = _current_type.value();
    node.set_from(from);

    if (!_can_implicit_convert(from, node.to())) {
        throw std::runtime_error("This cast is not valid.");
    }

    _current_type = node.to();
}

void TypeResolver::visit(CallNode &node) {
    auto function = std::get<FunctionSymbol>(*node.symbol());

    if (function.parameter_symbols().size() != node.arguments().size()) {
        throw std::runtime_error("The argument count doesn't equal to the parameters count.");
    }

    for (size_t index = 0; index < node.arguments().size(); index++) {
        const auto &parameter = std::get<ParameterSymbol>(*function.parameter_symbols()[index]);

        auto &argument = node.arguments()[index];
        argument->accept(*this);
        auto type = _current_type.value();

        if (type == parameter.type()) continue;

        if (!_can_implicit_convert(type, *parameter.type())) {
            throw std::runtime_error("The arguments type doesn't match the parameters one.");
        }

        // Replace the argument with its implicit conversion.
        node.arguments()[index] = std::make_unique<CastNode>(std::move(argument), type, *parameter.type());
    }

    _current_type = function.return_type();
}

void TypeResolver::visit(IfNode &node) {
    // This will set _current_type
    node.condition()->accept(*this);
    auto type = _current_type.value();

    if (!_can_implicit_convert(type, BOOL_TYPE)) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    if (!std::holds_alternative<BooleanType>(type)) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &expression = const_cast<std::unique_ptr<Node> &>(node.condition());
        node.set_condition(std::make_unique<CastNode>(std::move(expression), type, BOOL_TYPE));
    }

    std::visit([&](const auto &value) { value->accept(*this); }, node.then());

    if (node.branch()) std::visit([&](const auto &value) { value->accept(*this); }, *node.branch());
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
Type TypeResolver::_arithmetic_conversion(const Type &left_type, const Type &right_type) {
    auto floating_left = std::get_if<FloatingType>(&left_type);
    auto floating_right = std::get_if<FloatingType>(&right_type);

    // Stage 4: If either operand is of floating-point type, the following rules are applied:
    if (floating_left || floating_right) {
        // If both operands have the same type, no further conversion will be performed.
        if (left_type == right_type) return left_type;

        // Otherwise, if one of the operands is of a non-floating-point type, that operand is converted to the type of
        // the other operand.
        if (floating_left && !floating_right) return left_type;
        if (floating_right && !floating_left) return right_type;

        // Otherwise, if the floating-point conversion ranks of the types of the operands are ordered but(since C++23)
        // not equal, then the operand of the type with the lesser floating-point conversion rank is converted to the
        // type of the other operand.
        if (floating_left->size() > floating_right->size()) return left_type;
        if (floating_right->size() > floating_left->size()) return right_type;
    }

    // Stage 5: Both operands are converted to a common op C.
    auto t1 = std::visit(match{
        [](const IntegralType &type) -> IntegralType { return type; },
        [](const BooleanType &) -> IntegralType { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> IntegralType { throw std::runtime_error("The left type must be of integral type."); }
    }, left_type);
    auto t2 = std::visit(match{
        [](const IntegralType &type) -> IntegralType { return type; },
        [](const BooleanType &) -> IntegralType { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> IntegralType { throw std::runtime_error("The left type must be of integral type."); }
    }, right_type);

    // Given the types T1 and T2 as the promoted op (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1.size() < Size::DWORD) t1 = IntegralType(Size::DWORD, t1.sign());
    if (t2.size() < Size::DWORD) t2 = IntegralType(Size::DWORD, t2.sign());

    // 1. If T1 and T2 are the same type, C is that op.
    if (t1 == t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the op of greater integer
    //    conversion rank.
    if (t1.sign() == t2.sign()) {
        if (t2.size() > t1.size()) return t2;
        else return t1;
    }

    // 3. Otherwise, one type between T1 and T2 is an signed integer type S, the other type is an unsigned integer op U.
    //    Apply the following rules:
    const auto &_signed = t1.sign() ? t1 : t2;
    const auto &_unsigned = !t1.sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned.size() >= _signed.size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all of the values of U, C is S.
    if (_signed.max() >= _unsigned.max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer op corresponding to S.
    return IntegralType(_signed.size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const Type &from, const Type &destination) {
    return std::visit(match{
        // A prvalue of an integer type or of an unscoped enumeration op can be converted to any other integer type.
        // If the conversion is listed under integral promotions, it is a promotion and not a conversion.
        [](const IntegralType &, const IntegralType &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of type bool.
        [](const IntegralType &, const BooleanType &) { return true; },
        // A prvalue of integer or unscoped enumeration type can be converted to a prvalue of any floating-point type.
        // The result is exact if possible.
        [](const IntegralType &, const FloatingType &) { return true; },
        // A prvalue of a floating-point type can be converted to a prvalue of any other floating-point type. (until C++23)
        [](const FloatingType &, const FloatingType &) { return true; },
        // A prvalue of floating-point type can be converted to a prvalue of any integer type. The fractional part is
        // truncated, that is, the fractional part is discarded.
        [](const FloatingType &, const IntegralType &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of type bool.
        [](const FloatingType &, const BooleanType &) { return true; },
        // If the source type is bool, the value false is converted to zero and the value true is converted to the value
        // one of the destination type (note that if the destination type is int, this is an integer promotion, not an
        // integer conversion).
        [](const BooleanType &, const IntegralType &) { return true; },
        // If the source type is bool, the value false is converted to zero, and the value true is converted to one.
        [](const BooleanType &, const FloatingType &) { return true; },
        [&](const auto &, const auto &) { return from == destination; },
    }, from, destination);
}
