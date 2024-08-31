#include "memory_resolver.h"

#include "instruction.h"

static Register INTEGER_ORDER[6] = {Register::RDI, Register::RSI, Register::RDX, Register::RCX, Register::R8,
                                    Register::R9};

void MemoryResolver::visit(LabelInstruction &) {}

void MemoryResolver::visit(BeginInstruction &node) {
    _current_begin = &node;
    _parameter_offset = 8;
    _offsets.clear();
}

void MemoryResolver::visit(ReturnInstruction &node) {
    node.set_value(_resolve_operand(node.value()));
}

void MemoryResolver::visit(BinaryInstruction &node) {
    node.set_result(_resolve_operand(node.result()));
    node.set_left(_resolve_operand(node.left()));
    node.set_right(_resolve_operand(node.right()));
}

void MemoryResolver::visit(EndInstruction &) {}

Operand MemoryResolver::_resolve_operand(const Operand &operand) {
    if (!std::holds_alternative<std::shared_ptr<Symbol>>(operand)) {
        return operand;
    }

    auto symbol = std::get<std::shared_ptr<Symbol>>(operand);
    if (std::holds_alternative<TemporarySymbol>(*symbol)) {
        auto result = _offsets.emplace(symbol, _current_begin->size() + 8);
        if (result.second) {
            _current_begin->increase_size(8);
        }

        return FPRelative(result.first->second, false);
    } else if (auto parameter = std::get_if<ParameterSymbol>(symbol.get())) {
        // TODO: This is only for integer typed parameters
        if (parameter->index() < 6) {
            auto reg = INTEGER_ORDER[parameter->index()];
            return reg;
        }

        auto result = _offsets.emplace(symbol, _parameter_offset + 8);
        if (result.second) {
            _parameter_offset += 8;
        }

        return FPRelative(result.first->second, true);
    }

    throw std::invalid_argument("Only parameter and temporary symbols are resolvable.");
}
