#include "sem/type_resolver.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "ast/nodes.hpp"

using namespace arkoi::sem;
using namespace arkoi;

static constinit Integral BOOL_PROMOTED_INT_TYPE = {Size::DWORD, false};
static constinit Boolean BOOL_TYPE = {};

TypeResolver TypeResolver::resolve(ast::Program &node) {
    TypeResolver resolver;

    node.accept(resolver);

    return resolver;
}

void TypeResolver::visit(ast::Program &node) {
    for (const auto &statement: node.statements()) {
        auto *function = dynamic_cast<ast::Function *>(statement.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &statement: node.statements()) {
        statement->accept(*this);
    }
}

void TypeResolver::visit_as_prototype(ast::Function &node) {
    for (auto &parameter: node.parameters()) {
        parameter.accept(*this);
    }

    auto &function = std::get<sem::Function>(*node.name().symbol());
    function.set_return_type(node.type());
}

void TypeResolver::visit(ast::Function &node) {
    auto &function = std::get<sem::Function>(*node.name().symbol());
    _return_type = function.return_type();

    node.block()->accept(*this);
}

void TypeResolver::visit(ast::Block &node) {
    for (const auto &statement: node.statements()) {
        statement->accept(*this);
    }
}

void TypeResolver::visit(ast::Parameter &node) {
    auto &variable = std::get<sem::Variable>(*node.name().symbol());
    variable.set_type(node.type());
}

void TypeResolver::visit(ast::Immediate &node) {
    switch (node.kind()) {
        case ast::Immediate::Kind::Integer: return visit_integer(node);
        case ast::Immediate::Kind::Floating: return visit_floating(node);
        case ast::Immediate::Kind::Boolean: return visit_boolean(node);
        default: throw std::runtime_error("This immediate type is not implemented yet.");
    }
}

void TypeResolver::visit_integer(ast::Immediate &node) {
    const auto &number_string = node.value().contents();
    auto sign = !number_string.starts_with('-');

    Size size;
    if (sign) {
        size = std::stoll(number_string) > std::numeric_limits<int32_t>::max() ? Size::QWORD : Size::DWORD;
    } else {
        size = std::stoull(number_string) > std::numeric_limits<uint32_t>::max() ? Size::QWORD : Size::DWORD;
    }

    node.set_type(Integral(size, sign));
    _current_type = node.type();
}

void TypeResolver::visit_floating(ast::Immediate &node) {
    const auto &number_string = node.value().contents();

    auto size = std::stold(number_string) > std::numeric_limits<float>::max() ? Size::QWORD : Size::DWORD;

    node.set_type(Floating(size));
    _current_type = node.type();
}

void TypeResolver::visit_boolean(ast::Immediate &node) {
    node.set_type(Boolean());
    _current_type = node.type();
}

void TypeResolver::visit(ast::Return &node) {
    node.expression()->accept(*this);
    auto type = _current_type.value();

    node.set_type(_return_type.value());

    if (type == _return_type.value()) {
        return;
    }

    if (!_can_implicit_convert(type, _return_type.value())) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    auto casted_expression = _cast(node.expression(), type, _return_type.value());
    node.set_expression(std::move(casted_expression));
}

void TypeResolver::visit(ast::Identifier &node) {
    if(node.kind() == ast::Identifier::Kind::Function) {
        auto &function = std::get<sem::Function>(*node.symbol());
        _current_type = function.return_type();
    } else if(node.kind() == ast::Identifier::Kind::Variable) {
        // TODO: In the future there will be local/global and parameter variables,
        //       thus they need to be searched in such order: local, parameter, global.
        //       For now only parameter variables exist.
        auto &variable = std::get<sem::Variable>(*node.symbol());
        _current_type = variable.type();
    } else {
        throw std::runtime_error("This kind of identifier is not yet implemented.");
    }
}

void TypeResolver::visit(ast::Binary &node) {
    // This will set _current_type
    node.left()->accept(*this);
    auto left = _current_type.value();

    // This will set _current_type
    node.right()->accept(*this);
    auto right = _current_type.value();

    auto op_type = _arithmetic_conversion(left, right);
    node.set_op_type(op_type);

    if (left != op_type) {
        auto casted_left = _cast(node.left(), left, op_type);
        node.set_left(std::move(casted_left));
    }

    if (right != op_type) {
        auto casted_right = _cast(node.right(), right, op_type);
        node.set_right(std::move(casted_right));
    }

    switch (node.op()) {
        // A logical operation always has the return result_type of BOOL_TYPE
        case ast::Binary::Operator::GreaterThan:
        case ast::Binary::Operator::LessThan: {
            _current_type = BOOL_TYPE;
            node.set_result_type(BOOL_TYPE);
            break;
        }

        // Already the right op_type result_type
        default: {
            _current_type = op_type;
            node.set_result_type(op_type);
            break;
        }
    }
}

void TypeResolver::visit(ast::Cast &node) {
    // This will set _current_type
    node.expression()->accept(*this);
    auto from = _current_type.value();
    node.set_from(from);

    if (!_can_implicit_convert(from, node.to())) {
        throw std::runtime_error("This cast is not valid.");
    }

    _current_type = node.to();
}

void TypeResolver::visit(ast::Assign &node) {
    node.name().accept(*this);
    auto identifier_type = _current_type.value();

    node.expression()->accept(*this);
    auto type = _current_type.value();

    if (!_can_implicit_convert(type, identifier_type)) {
        throw std::runtime_error("Assign source has a wrong type.");
    }

    if (type != identifier_type) {
        auto casted_expression = _cast(node.expression(), type, identifier_type);
        node.set_expression(std::move(casted_expression));
    }
}

void TypeResolver::visit(ast::Call &node) {
    node.name().accept(*this);

    auto function = std::get<sem::Function>(*node.name().symbol());

    if (function.parameters().size() != node.arguments().size()) {
        throw std::runtime_error("The argument count doesn't equal to the parameters count.");
    }

    for (size_t index = 0; index < node.arguments().size(); index++) {
        const auto &variable = function.parameters()[index];

        auto &argument = node.arguments()[index];
        argument->accept(*this);
        auto type = _current_type.value();

        if (type == variable->type()) continue;

        if (!_can_implicit_convert(type, variable->type())) {
            throw std::runtime_error("The arguments type doesn't match the parameters one.");
        }

        // Replace the argument with its implicit conversion.
        auto casted_argument = _cast(argument, type, variable->type());
        node.arguments()[index] = std::move(casted_argument);
    }

    _current_type = function.return_type();
}

void TypeResolver::visit(ast::If &node) {
    // This will set _current_type
    node.condition()->accept(*this);
    auto type = _current_type.value();

    if (!_can_implicit_convert(type, BOOL_TYPE)) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    if (!std::holds_alternative<Boolean>(type)) {
        auto casted_condition = _cast(node.condition(), type, BOOL_TYPE);
        node.set_condition(std::move(casted_condition));
    }

    node.branch()->accept(*this);

    if (node.next()) node.next()->accept(*this);
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
Type TypeResolver::_arithmetic_conversion(const Type &left_type, const Type &right_type) {
    auto floating_left = std::get_if<Floating>(&left_type);
    auto floating_right = std::get_if<Floating>(&right_type);

    // Stage 4: If either operand is of floating-point type, the following rules are applied:
    if (floating_left || floating_right) {
        // If both operands have the same type, no further conversion will be performed.
        if (left_type == right_type) return left_type;

        // Otherwise, if one of the operands is of a non-floating-point type, that operand is converted to the mid of
        // the other operand.
        if (floating_left && !floating_right) return left_type;
        if (floating_right && !floating_left) return right_type;

        // Otherwise, if the floating-point conversion ranks of the types of the operands are ordered but(since C++23)
        // not equal, then the operand of the mid with the lesser floating-point conversion rank is converted to the
        // mid of the other operand.
        if (floating_left->size() > floating_right->size()) return left_type;
        if (floating_right->size() > floating_left->size()) return right_type;
    }

    // Stage 5: Both operands are converted to a common op C.
    auto t1 = std::visit(match{
        [](const Integral &type) -> Integral { return type; },
        [](const Boolean &) -> Integral { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> Integral { throw std::runtime_error("The left type must be of integral type."); }
    }, left_type);
    auto t2 = std::visit(match{
        [](const Integral &type) -> Integral { return type; },
        [](const Boolean &) -> Integral { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> Integral { throw std::runtime_error("The left type must be of integral type."); }
    }, right_type);

    // Given the types T1 and T2 as the promoted op (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1.size() < Size::DWORD) t1 = Integral(Size::DWORD, t1.sign());
    if (t2.size() < Size::DWORD) t2 = Integral(Size::DWORD, t2.sign());

    // 1. If T1 and T2 are the same type, C is that op.
    if (t1 == t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the op of greater integer
    //    conversion rank.
    if (t1.sign() == t2.sign()) {
        if (t2.size() > t1.size()) return t2;
        else return t1;
    }

    // 3. Otherwise, one mid between T1 and T2 is an signed integer mid S, the other type is an unsigned integer op U.
    //    Apply the following rules:
    const auto &_signed = t1.sign() ? t1 : t2;
    const auto &_unsigned = !t1.sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned.size() >= _signed.size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all of the values of U, C is S.
    if (_signed.max() >= _unsigned.max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer op corresponding to S.
    return Integral(_signed.size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const Type &from, const Type &destination) {
    return std::visit(match{
        // A prvalue of an integer mid or of an unscoped enumeration op can be converted to any other integer mid.
        // If the conversion is listed under integral promotions, it is a promotion and not a conversion.
        [](const Integral &, const Integral &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of mid bool.
        [](const Integral &, const Boolean &) { return true; },
        // A prvalue of integer or unscoped enumeration mid can be converted to a prvalue of any floating-point mid.
        // The result is exact if possible.
        [](const Integral &, const Floating &) { return true; },
        // A prvalue of a floating-point mid can be converted to a prvalue of any other floating-point mid. (until C++23)
        [](const Floating &, const Floating &) { return true; },
        // A prvalue of floating-point mid can be converted to a prvalue of any integer mid. The fractional part is
        // truncated, that is, the fractional part is discarded.
        [](const Floating &, const Integral &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of mid bool.
        [](const Floating &, const Boolean &) { return true; },
        // If the source mid is bool, the value false is converted to zero and the value true is converted to the value
        // one of the destination mid (note that if the destination mid is int, this is an integer promotion, not an
        // integer conversion).
        [](const Boolean &, const Integral &) { return true; },
        // If the source mid is bool, the value false is converted to zero, and the value true is converted to one.
        [](const Boolean &, const Floating &) { return true; },
        [&](const auto &, const auto &) { return from == destination; },
    }, from, destination);
}

std::unique_ptr<ast::Node> TypeResolver::_cast(std::unique_ptr<ast::Node> &node, const Type &from, const Type &to) {
    return std::make_unique<ast::Cast>(std::move(node), from, to);
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
