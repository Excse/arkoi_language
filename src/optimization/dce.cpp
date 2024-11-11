#include "optimization/dce.hpp"

#include <ranges>

#include "utils/utils.hpp"

bool DeadCodeElimination::new_function(Function &) {
    _used_variables.clear();
    return false;
}

bool DeadCodeElimination::new_block(BasicBlock &block) {
    bool changed = false;

    changed |= _eliminate_dead_stores(block);

    return changed;
}

bool DeadCodeElimination::_eliminate_dead_stores(BasicBlock &block) {
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
            [&](const il::Label &) { is_unreachable = false; },
            [&](const il::Begin &) { is_unreachable = false; },
            [&](const il::Return &) { is_unreachable = true; },
            [](const auto &) {}
        }, instruction);

        has_changed |= is_unnecessary;
        return is_unnecessary;
    });

    return has_changed;
}

void DeadCodeElimination::_mark_variable(const il::Operand &operand) {
    const auto *variable = std::get_if<il::Variable>(&operand);
    if (variable == nullptr) return;

    _used_variables.insert(variable);
}

void DeadCodeElimination::_mark_instruction(const il::InstructionType &type) {
    std::visit(match{
        [&](il::Return &instruction) {
            _mark_variable(instruction.value());
        },
        [&](il::Cast &instruction) {
            _mark_variable(instruction.expression());
        },
        [&](il::If &instruction) {
            _mark_variable(instruction.condition());
        },
        [&](il::Binary &instruction) {
            _mark_variable(instruction.left());
            _mark_variable(instruction.right());
        },
        [&](il::Call &instruction) {
            for (auto &argument: instruction.arguments()) {
                _mark_variable(argument);
            }
        },
        [](const auto &) {}
    }, type);
}

bool DeadCodeElimination::_is_dead_store(const il::InstructionType &type) {
    if (!std::holds_alternative<il::Store>(type)) return false;

    auto store = std::get<il::Store>(type);
    if (store.has_side_effects()) return false;

    return !_used_variables.contains(&store.result());
}
