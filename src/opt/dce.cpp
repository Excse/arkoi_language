#include "opt/dce.hpp"

#include <ranges>

#include "utils/utils.hpp"

using namespace arkoi::opt;

bool DeadCodeElimination::new_function(mid::Function &) {
    _used_variables.clear();
    return false;
}

bool DeadCodeElimination::new_block(mid::BasicBlock &block) {
    bool changed = false;

    changed |= _eliminate_dead_stores(block);

    return changed;
}

bool DeadCodeElimination::_eliminate_dead_stores(mid::BasicBlock &block) {
    for (const auto &instruction: block.instructions()) {
        _mark_instruction(instruction);
    }

    bool is_unreachable = false;
    bool has_changed = false;

    std::erase_if(block.instructions(), [&](const auto &instruction) {
        auto is_unnecessary = false;

        is_unnecessary |= _is_dead_store(instruction);
        is_unnecessary |= is_unreachable;

        std::visit(match{
            [&](const mid::Label &) { is_unreachable = false; },
            [&](const mid::Return &) { is_unreachable = true; },
            [](const auto &) {}
        }, instruction);

        has_changed |= is_unnecessary;
        return is_unnecessary;
    });

    return has_changed;
}

void DeadCodeElimination::_mark_variable(const mid::Operand &operand) {
    const auto *variable = std::get_if<mid::Variable>(&operand);
    if (variable == nullptr) return;

    _used_variables.insert(variable);
}

void DeadCodeElimination::_mark_instruction(const mid::InstructionType &type) {
    std::visit(match{
        [&](mid::Return &instruction) {
            _mark_variable(instruction.value());
        },
        [&](mid::Cast &instruction) {
            _mark_variable(instruction.expression());
        },
        [&](mid::If &instruction) {
            _mark_variable(instruction.condition());
        },
        [&](mid::Binary &instruction) {
            _mark_variable(instruction.left());
            _mark_variable(instruction.right());
        },
        [&](mid::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                _mark_variable(argument);
            }
        },
        [](const auto &) {}
    }, type);
}

bool DeadCodeElimination::_is_dead_store(const mid::InstructionType &type) {
    if (!std::holds_alternative<mid::Store>(type)) return false;

    auto store = std::get<mid::Store>(type);
    if (store.has_side_effects()) return false;

    return !_used_variables.contains(&store.result());
}
