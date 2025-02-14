#include "mid/type_resolver.hpp"

#include <limits>

#include "utils/utils.hpp"
#include "def/ast.hpp"

using namespace arkoi::mid;
using namespace arkoi;

static constinit type::Integral BOOL_PROMOTED_INT_TYPE = {Size::DWORD, false};
static constinit type::Boolean BOOL_TYPE = {};

TypeResolver TypeResolver::resolve(ast::Program &node) {
    TypeResolver resolver;

    node.accept(resolver);

    return resolver;
}

void TypeResolver::visit(ast::Program &node) {
    for (const auto &item: node.statements()) {
        auto *function = dynamic_cast<ast::Function *>(item.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit_as_prototype(ast::Function &node) {
    for (auto &item: node.parameters()) {
        item.accept(*this);
    }

    auto &function = std::get<symbol::Function>(*node.name().symbol());
    function.set_return_type(node.type());
}

void TypeResolver::visit(ast::Function &node) {
    auto &function = std::get<symbol::Function>(*node.name().symbol());
    _return_type = function.return_type();

    node.block()->accept(*this);
}

void TypeResolver::visit(ast::Block &node) {
    for (const auto &item: node.statements()) {
        item->accept(*this);
    }
}

void TypeResolver::visit(ast::Parameter &node) {
    auto &parameter = std::get<symbol::Parameter>(*node.name().symbol());
    parameter.set_type(node.type());
}

void TypeResolver::visit(ast::Integer &node) {
    const auto &number_string = node.value().contents();
    auto sign = !number_string.starts_with('-');

    Size size;
    if (sign) {
        size = std::stoll(number_string) > std::numeric_limits<int32_t>::max() ? Size::QWORD : Size::DWORD;
    } else {
        size = std::stoull(number_string) > std::numeric_limits<uint32_t>::max() ? Size::QWORD : Size::DWORD;
    }

    _current_type = type::Integral(size, sign);
}

void TypeResolver::visit(ast::Floating &node) {
    const auto &number_string = node.value().contents();

    auto size = std::stold(number_string) > std::numeric_limits<float>::max() ? Size::QWORD : Size::DWORD;
    _current_type = type::Floating(size);
}

void TypeResolver::visit(ast::Boolean &) {
    _current_type = type::Boolean();
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

    // We assure to override the const casted node with a new node. Thus, this exception is legal.
    auto &expression = const_cast<std::unique_ptr<ast::Node> &>(node.expression());
    node.set_expression(std::make_unique<ast::Cast>(std::move(expression), type, _return_type.value()));
}

void TypeResolver::visit(ast::Identifier &node) {
    if(node.kind() == ast::Identifier::Kind::Function) {
        const auto &function = std::get<symbol::Function>(*node.symbol());
        _current_type = function.return_type();
    } else if(node.kind() == ast::Identifier::Kind::Variable) {
        // TODO: In the future there will be local/global and parameter variables,
        //       thus they need to be searched in such order: local, parameter, global.
        //       For now only parameter variables exist.
        const auto &parameter = std::get<symbol::Parameter>(*node.symbol());
        _current_type = parameter.type();
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
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &left_node = const_cast<std::unique_ptr<ast::Node> &>(node.left());
        node.set_left(std::make_unique<ast::Cast>(std::move(left_node), left, op_type));
    }

    if (right != op_type) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &right_node = const_cast<std::unique_ptr<ast::Node> &>(node.right());
        node.set_right(std::make_unique<ast::Cast>(std::move(right_node), right, op_type));
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
        throw std::runtime_error("Assign expression has a wrong result_type.");
    }

    // We assure to override the const casted node with a new node. Thus, this exception is legal.
    auto &expression = const_cast<std::unique_ptr<ast::Node> &>(node.expression());
    node.set_expression(std::make_unique<ast::Cast>(std::move(expression), type, identifier_type));
}

void TypeResolver::visit(ast::Call &node) {
    node.name().accept(*this);

    auto function = std::get<symbol::Function>(*node.name().symbol());

    if (function.parameter_symbols().size() != node.arguments().size()) {
        throw std::runtime_error("The argument count doesn't equal to the parameters count.");
    }

    for (size_t index = 0; index < node.arguments().size(); index++) {
        const auto &parameter = std::get<symbol::Parameter>(*function.parameter_symbols()[index]);

        auto &argument = node.arguments()[index];
        argument->accept(*this);
        auto type = _current_type.value();

        if (type == parameter.type()) continue;

        if (!_can_implicit_convert(type, *parameter.type())) {
            throw std::runtime_error("The arguments result_type doesn't match the parameters one.");
        }

        // Replace the argument with its implicit conversion.
        node.arguments()[index] = std::make_unique<ast::Cast>(std::move(argument), type, *parameter.type());
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

    if (!std::holds_alternative<type::Boolean>(type)) {
        // We assure to override the const casted node with a new node. Thus, this exception is legal.
        auto &expression = const_cast<std::unique_ptr<ast::Node> &>(node.condition());
        node.set_condition(std::make_unique<ast::Cast>(std::move(expression), type, BOOL_TYPE));
    }

    std::visit([&](const auto &value) { value->accept(*this); }, node.branch());

    if (node.next()) std::visit([&](const auto &value) { value->accept(*this); }, *node.next());
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
Type TypeResolver::_arithmetic_conversion(const Type &left_type, const Type &right_type) {
    auto floating_left = std::get_if<type::Floating>(&left_type);
    auto floating_right = std::get_if<type::Floating>(&right_type);

    // Stage 4: If either operand is of floating-point result_type, the following rules are applied:
    if (floating_left || floating_right) {
        // If both operands have the same result_type, no further conversion will be performed.
        if (left_type == right_type) return left_type;

        // Otherwise, if one of the operands is of a non-floating-point result_type, that operand is converted to the mid of
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
        [](const type::Integral &type) -> type::Integral { return type; },
        [](const type::Boolean &) -> type::Integral { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> type::Integral { throw std::runtime_error("The left result_type must be of integral result_type."); }
    }, left_type);
    auto t2 = std::visit(match{
        [](const type::Integral &type) -> type::Integral { return type; },
        [](const type::Boolean &) -> type::Integral { return BOOL_PROMOTED_INT_TYPE; },
        [](const auto &) -> type::Integral { throw std::runtime_error("The left result_type must be of integral result_type."); }
    }, right_type);

    // Given the types T1 and T2 as the promoted op (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1.size() < Size::DWORD) t1 = type::Integral(Size::DWORD, t1.sign());
    if (t2.size() < Size::DWORD) t2 = type::Integral(Size::DWORD, t2.sign());

    // 1. If T1 and T2 are the same result_type, C is that op.
    if (t1 == t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the op of greater integer
    //    conversion rank.
    if (t1.sign() == t2.sign()) {
        if (t2.size() > t1.size()) return t2;
        else return t1;
    }

    // 3. Otherwise, one mid between T1 and T2 is an signed integer mid S, the other result_type is an unsigned integer op U.
    //    Apply the following rules:
    const auto &_signed = t1.sign() ? t1 : t2;
    const auto &_unsigned = !t1.sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned.size() >= _signed.size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all of the values of U, C is S.
    if (_signed.max() >= _unsigned.max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer op corresponding to S.
    return type::Integral(_signed.size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const Type &from, const Type &destination) {
    return std::visit(match{
        // A prvalue of an integer mid or of an unscoped enumeration op can be converted to any other integer mid.
        // If the conversion is listed under integral promotions, it is a promotion and not a conversion.
        [](const type::Integral &, const type::Integral &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of mid bool.
        [](const type::Integral &, const type::Boolean &) { return true; },
        // A prvalue of integer or unscoped enumeration mid can be converted to a prvalue of any floating-point mid.
        // The result is exact if possible.
        [](const type::Integral &, const type::Floating &) { return true; },
        // A prvalue of a floating-point mid can be converted to a prvalue of any other floating-point mid. (until C++23)
        [](const type::Floating &, const type::Floating &) { return true; },
        // A prvalue of floating-point mid can be converted to a prvalue of any integer mid. The fractional part is
        // truncated, that is, the fractional part is discarded.
        [](const type::Floating &, const type::Integral &) { return true; },
        // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
        // converted to a prvalue of mid bool.
        [](const type::Floating &, const type::Boolean &) { return true; },
        // If the source mid is bool, the value false is converted to zero and the value true is converted to the value
        // one of the destination mid (note that if the destination mid is int, this is an integer promotion, not an
        // integer conversion).
        [](const type::Boolean &, const type::Integral &) { return true; },
        // If the source mid is bool, the value false is converted to zero, and the value true is converted to one.
        [](const type::Boolean &, const type::Floating &) { return true; },
        [&](const auto &, const auto &) { return from == destination; },
    }, from, destination);
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
