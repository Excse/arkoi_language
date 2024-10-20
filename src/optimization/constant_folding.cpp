#include "optimization/constant_folding.hpp"

#include "utils/utils.hpp"

bool ConstantFolding::new_block(BasicBlock &block) {
    auto changed = false;

    for (auto &instruction: block.instructions()) {
        std::optional<std::unique_ptr<Instruction>> new_instruction;

        if (auto binary = dynamic_cast<il::Binary *>(instruction.get())) {
            new_instruction = _binary(*binary);
        } else if (auto cast = dynamic_cast<il::Cast *>(instruction.get())) {
            new_instruction = _cast(*cast);
        }

        if (!new_instruction) continue;

        instruction = std::move(*new_instruction);
        changed = true;
    }

    return changed;
}

std::optional<std::unique_ptr<Instruction>> ConstantFolding::_binary(const il::Binary &instruction) {
    auto *right_constant = std::get_if<il::Constant>(&instruction.right());
    auto *left_constant = std::get_if<il::Constant>(&instruction.left());

    if (!right_constant || !left_constant) return std::nullopt;

    auto apply_operator = [&](auto left, auto right) -> il::Operand {
        switch (instruction.op()) {
            case il::Binary::Operator::Add: return left + right;
            case il::Binary::Operator::Sub: return left - right;
            case il::Binary::Operator::Mul: return left * right;
            case il::Binary::Operator::Div: return left / right;
        }

        // As the -Wswitch flag is set, this will never be reached.
        std::unreachable();
    };

    auto value = std::visit(match{
        [&](const double &left, const double &right) -> il::Operand { return apply_operator(left, right); },
        [&](const float &left, const float &right) -> il::Operand { return apply_operator(left, right); },
        [&](const int32_t &left, const int32_t &right) -> il::Operand { return apply_operator(left, right); },
        [&](const uint32_t &left, const uint32_t &right) -> il::Operand { return apply_operator(left, right); },
        [&](const int64_t &left, const int64_t &right) -> il::Operand { return apply_operator(left, right); },
        [&](const uint64_t &left, const uint64_t &right) -> il::Operand { return apply_operator(left, right); },
        [&](const bool &left, const bool &right) -> il::Operand { return apply_operator(left, right); },
        [](const auto &, const auto &) -> il::Operand { std::unreachable(); }
    }, *right_constant, *left_constant);

    return std::make_unique<il::Store>(instruction.result(), value, instruction.type());
}

std::optional<std::unique_ptr<Instruction>> ConstantFolding::_cast(const il::Cast &instruction) {
    auto *expression = std::get_if<il::Constant>(&instruction.expression());

    if (!expression) return std::nullopt;

    auto apply_operator = [&](auto expression, const Type &to) -> il::Operand {
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
            [&](const type::Boolean &) -> il::Operand { return (bool) expression; }
        }, to);
    };

    auto value = std::visit([&](const auto &value) -> il::Operand {
        return apply_operator(value, instruction.to());
    }, *expression);

    return std::make_unique<il::Store>(instruction.result(), value, instruction.to());
}
