#include "optimization/constant_propagation.hpp"

bool ConstantPropagation::new_function(Function &) {
    _constants.clear();
    return false;
}

bool ConstantPropagation::new_block(BasicBlock &block) {
    auto changed = false;

    for (auto &instruction: block.instructions()) {
        auto *store = dynamic_cast<il::Store *>(instruction.get());
        if (store == nullptr) continue;

        if(store->has_side_effects()) continue;

        auto *constant = std::get_if<il::Constant>(&store->value());
        if (constant == nullptr) continue;

        _constants[store->result()] = *constant;
    }

    for (auto &instruction: block.instructions()) {
        if (auto *_binary = dynamic_cast<il::Binary *>(instruction.get())) {
            _propagate(_binary->left());
            _propagate(_binary->right());
        } else if (auto *_return = dynamic_cast<il::Return *>(instruction.get())) {
            _propagate(_return->value());
        } else if (auto *_cast = dynamic_cast<il::Cast *>(instruction.get())) {
            _propagate(_cast->expression());
        } else if (auto *_call = dynamic_cast<il::Call *>(instruction.get())) {
            for (auto &argument: _call->arguments()) {
                _propagate(argument);
            }
        } else if (auto *_if = dynamic_cast<il::If *>(instruction.get())) {
            _propagate(_if->condition());
        }
    }

    return changed;
}

void ConstantPropagation::_propagate(il::Operand &operand) {
    auto condition_symbol = std::get_if<il::Variable>(&operand);
    if (condition_symbol == nullptr) return;

    auto result = _constants.find(*condition_symbol);
    if (result == _constants.end()) return;

    operand = result->second;
}
