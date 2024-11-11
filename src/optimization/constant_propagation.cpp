#include "optimization/constant_propagation.hpp"

#include <ranges>

#include "utils/utils.hpp"

bool ConstantPropagation::new_function(Function &) {
    _constants.clear();
    return false;
}

bool ConstantPropagation::new_block(BasicBlock &block) {
    auto has_changed = false;

    for (auto &instruction: block.instructions()) {
        _add_constant(instruction);

        has_changed |= _can_propagate(instruction);
    }

    return has_changed;
}

bool ConstantPropagation::_propagate(il::Operand &operand) {
    auto condition_symbol = std::get_if<il::Variable>(&operand);
    if (condition_symbol == nullptr) return false;

    auto result = _constants.find(*condition_symbol);
    if (result == _constants.end()) return false;

    operand = result->second;
    return true;
}

void ConstantPropagation::_add_constant(il::InstructionType &type) {
    if (!std::holds_alternative<il::Store>(type)) return;

    auto &store = std::get<il::Store>(type);
    if (store.has_side_effects()) return;

    auto *constant = std::get_if<il::Constant>(&store.value());
    if (constant == nullptr) return;

    _constants[store.result()] = *constant;
}

bool ConstantPropagation::_can_propagate(il::InstructionType &type) {
    auto propagated = false;

    std::visit(match{
        [&](il::Binary &instruction) {
            propagated |= _propagate(instruction.left());
            propagated |= _propagate(instruction.right());
        },
        [&](il::Return &instruction) {
            propagated |= _propagate(instruction.value());
        },
        [&](il::Cast &instruction) {
            propagated |= _propagate(instruction.expression());
        },
        [&](il::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                propagated |= _propagate(argument);
            }
        },
        [&](il::If &instruction) {
            propagated |= _propagate(instruction.condition());
        },
        [](const auto &) {}
    }, type);

    return propagated;
}
