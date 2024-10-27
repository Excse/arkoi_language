#include "optimization/dce.hpp"

bool DeadCodeElimination::new_function(Function &) {
    _used.clear();
    return false;
}

bool DeadCodeElimination::new_block(BasicBlock &block) {
    bool changed = false;

    changed |= _eliminate_dead_stores(block);

    return changed;
}

bool DeadCodeElimination::_eliminate_dead_stores(BasicBlock &block) {
    bool changed = false;

    UsedVariables used;
    for (auto &instruction: block.instructions()) {
        if (auto *_binary = dynamic_cast<il::Binary *>(instruction.get())) {
            _mark_as_used(_binary->left());
            _mark_as_used(_binary->right());
        } else if (auto *_return = dynamic_cast<il::Return *>(instruction.get())) {
            _mark_as_used(_return->value());
        } else if (auto *_cast = dynamic_cast<il::Cast *>(instruction.get())) {
            _mark_as_used(_cast->expression());
        } else if (auto *_call = dynamic_cast<il::Call *>(instruction.get())) {
            for (const auto &argument: _call->arguments()) {
                _mark_as_used(argument);
            }
        } else if (auto *_if = dynamic_cast<il::If *>(instruction.get())) {
            _mark_as_used(_if->condition());
        }
    }

    for (auto iterator = block.instructions().begin(); iterator != block.instructions().end(); iterator++) {
        auto &instruction = *iterator;

        auto *store = dynamic_cast<il::Store *>(instruction.get());
        if (store == nullptr) continue;

        if (store->has_side_effects()) continue;

        if (_used.contains(&store->result())) continue;

        block.instructions().erase(iterator);
        iterator--;

        changed = true;
    }

    return changed;
}

void DeadCodeElimination::_mark_as_used(const il::Operand &operand) {
    const auto *variable = std::get_if<il::Variable>(&operand);
    if (variable == nullptr) return;
    _used.insert(variable);
}
