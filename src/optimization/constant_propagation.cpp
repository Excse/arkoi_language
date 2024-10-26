#include "optimization/constant_propagation.hpp"

bool ConstantPropagation::new_block(BasicBlock &block) {
    auto changed = false;

    Constants constants;
    for (auto &instruction: block.instructions()) {
        auto *store = dynamic_cast<il::Store *>(instruction.get());
        if (store == nullptr) continue;

        auto *constant = std::get_if<il::Constant>(&store->value());
        if (constant == nullptr) continue;

        constants[store->result()] = *constant;
    }

    for (auto &instruction: block.instructions()) {
        if (auto *_binary = dynamic_cast<il::Binary *>(instruction.get())) _propagate_binary(*_binary, constants);
        if (auto *_return = dynamic_cast<il::Return *>(instruction.get())) _propagate_return(*_return, constants);
        if (auto *_cast = dynamic_cast<il::Cast *>(instruction.get())) _propagate_cast(*_cast, constants);
        if (auto *_call = dynamic_cast<il::Call *>(instruction.get())) _propagate_call(*_call, constants);
        if (auto *_if = dynamic_cast<il::If *>(instruction.get())) _propagate_if(*_if, constants);
    }

    return changed;
}

void ConstantPropagation::_propagate_if(il::If &instruction, const ConstantPropagation::Constants &constants) {
    auto condition_symbol = std::get_if<il::Variable>(&instruction.condition());
    if (condition_symbol == nullptr) return;

    auto result = constants.find(*condition_symbol);
    if (result == constants.end()) return;

    instruction.set_condition(result->second);
}

void ConstantPropagation::_propagate_call(il::Call &instruction, const ConstantPropagation::Constants &constants) {
    for (auto &argument: instruction.arguments()) {
        auto argument_symbol = std::get_if<il::Variable>(&argument);
        if (argument_symbol == nullptr) continue;

        auto result = constants.find(*argument_symbol);
        if (result == constants.end()) continue;

        argument = result->second;
    }
}

void ConstantPropagation::_propagate_return(il::Return &instruction, const ConstantPropagation::Constants &constants) {
    auto condition_symbol = std::get_if<il::Variable>(&instruction.value());
    if (condition_symbol == nullptr) return;

    auto result = constants.find(*condition_symbol);
    if (result == constants.end()) return;

    instruction.set_value(result->second);
}

void ConstantPropagation::_propagate_binary(il::Binary &instruction, const ConstantPropagation::Constants &constants) {
    auto left_symbol = std::get_if<il::Variable>(&instruction.left());
    if (left_symbol && constants.contains(*left_symbol)) {
        auto constant = constants.at(*left_symbol);
        instruction.set_left(constant);
    }

    auto right_symbol = std::get_if<il::Variable>(&instruction.right());
    if (right_symbol && constants.contains(*right_symbol)) {
        auto constant = constants.at(*right_symbol);
        instruction.set_right(constant);
    }
}

void ConstantPropagation::_propagate_cast(il::Cast &instruction, const ConstantPropagation::Constants &constants) {
    auto expression_symbol = std::get_if<il::Variable>(&instruction.expression());
    if (expression_symbol == nullptr) return;

    auto result = constants.find(*expression_symbol);
    if (result == constants.end()) return;

    instruction.set_expression(result->second);
}
