#include "opt/constant_propagation.hpp"

#include <ranges>

#include "utils/utils.hpp"

using namespace arkoi::opt;

bool ConstantPropagation::new_function(mid::Function &) {
    _constants.clear();
    return false;
}

bool ConstantPropagation::new_block(mid::BasicBlock &block) {
    auto has_changed = false;

    for (auto &instruction: block.instructions()) {
        _add_constant(instruction);

        has_changed |= _can_propagate(instruction);
    }

    return has_changed;
}

bool ConstantPropagation::_propagate(mid::Operand &operand) {
    auto condition_symbol = std::get_if<mid::Variable>(&operand);
    if (condition_symbol == nullptr) return false;

    auto result = _constants.find(*condition_symbol);
    if (result == _constants.end()) return false;

    operand = result->second;
    return true;
}

void ConstantPropagation::_add_constant(mid::InstructionType &type) {
    if (!std::holds_alternative<mid::Store>(type)) return;

    auto &store = std::get<mid::Store>(type);
    if (store.has_side_effects()) return;

    auto *constant = std::get_if<mid::Constant>(&store.value());
    if (constant == nullptr) return;

    _constants[store.result()] = *constant;
}

bool ConstantPropagation::_can_propagate(mid::InstructionType &type) {
    auto propagated = false;

    std::visit(match{
        [&](mid::Binary &instruction) {
            propagated |= _propagate(instruction.left());
            propagated |= _propagate(instruction.right());
        },
        [&](mid::Return &instruction) {
            propagated |= _propagate(instruction.value());
        },
        [&](mid::Cast &instruction) {
            propagated |= _propagate(instruction.expression());
        },
        [&](mid::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                propagated |= _propagate(argument);
            }
        },
        [&](mid::If &instruction) {
            propagated |= _propagate(instruction.condition());
        },
        [](const auto &) {}
    }, type);

    return propagated;
}
