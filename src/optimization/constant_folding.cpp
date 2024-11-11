#include "optimization/constant_folding.hpp"

#include "utils/utils.hpp"
#include "il/operand.hpp"

bool ConstantFolding::new_block(BasicBlock &block) {
    auto has_changed = false;

    for (auto &instruction: block.instructions()) {
        if (auto binary = std::get_if<il::Binary>(&instruction)) {
            auto result = _binary(*binary);
            if (!result.has_value()) continue;

            instruction = *result;
            has_changed = true;
        } else if (auto cast = std::get_if<il::Cast>(&instruction)) {
            auto result = _cast(*cast);
            if (!result.has_value()) continue;

            instruction = *result;
            has_changed = true;
        }
    }

    return has_changed;
}

std::optional<il::InstructionType> ConstantFolding::_binary(const il::Binary &instruction) {
    auto *right_constant = std::get_if<il::Constant>(&instruction.right());
    auto *left_constant = std::get_if<il::Constant>(&instruction.left());

    if (!right_constant || !left_constant) return std::nullopt;

    auto value = std::visit([&](const auto &left, const auto &right) {
        return _evaluate_binary(instruction, left, right);
    }, *right_constant, *left_constant);

    return il::Store(instruction.result(), value, instruction.type());
}

il::Operand ConstantFolding::_evaluate_binary(const il::Binary &instruction, auto left, auto right) {
    switch (instruction.op()) {
        case il::Binary::Operator::Add: return left + right;
        case il::Binary::Operator::Sub: return left - right;
        case il::Binary::Operator::Mul: return left * right;
        case il::Binary::Operator::Div: return left / right;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::optional<il::InstructionType> ConstantFolding::_cast(const il::Cast &instruction) {
    auto *expression = std::get_if<il::Constant>(&instruction.expression());

    if (!expression) return std::nullopt;

    auto value = std::visit([&](const auto &value) {
        return _evaluate_unary(instruction.to(), value);
    }, *expression);

    return il::Store(instruction.result(), value, instruction.to());
}

il::Operand ConstantFolding::_evaluate_unary(const Type &to, auto expression) {
    return std::visit(match{
        [&](const type::Integral &type) -> il::Operand {
            switch (type.size()) {
                case Size::BYTE: return type.sign() ? (int8_t) expression : (uint8_t) expression;
                case Size::WORD: return type.sign() ? (int16_t) expression : (uint16_t) expression;
                case Size::DWORD: return type.sign() ? (int32_t) expression : (uint32_t) expression;
                case Size::QWORD: return type.sign() ? (int64_t) expression : (uint64_t) expression;
                default: std::unreachable();
            }
        },
        [&](const type::Floating &type) -> il::Operand {
            switch (type.size()) {
                case Size::DWORD: return (float) expression;
                case Size::QWORD: return (double) expression;
                default: std::unreachable();
            }
        },
        [&](const type::Boolean &) -> il::Operand {
            return (bool) expression;
        }
    }, to);
}
