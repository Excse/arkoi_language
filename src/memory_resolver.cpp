#include "memory_resolver.h"

#include "instruction.h"

static Register INTEGER_ORDER[6] = {Register::RDI, Register::RSI, Register::RDX, Register::RCX, Register::R8,
                                    Register::R9};

void MemoryResolver::visit(BeginInstruction &instruction) {
    _current_begin = &instruction;
    _parameter_offset = 8;
}

void MemoryResolver::visit(ReturnInstruction &instruction) {
    instruction.set_value(_resolve_operand(instruction.value()));
}

void MemoryResolver::visit(BinaryInstruction &instruction) {
    instruction.set_result(_resolve_operand(instruction.result()));
    instruction.set_left(_resolve_operand(instruction.left()));
    instruction.set_right(_resolve_operand(instruction.right()));
}

void MemoryResolver::visit(EndInstruction &) {}

Operand MemoryResolver::_resolve_operand(const Operand &operand) {
    if (!std::holds_alternative<std::shared_ptr<Symbol>>(operand)) {
        return operand;
    }

    auto symbol = std::get<std::shared_ptr<Symbol>>(operand);
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) {
        return result->second;
    }

    if (std::dynamic_pointer_cast<TemporarySymbol>(symbol)) {
        _current_begin->increase_size(8);

        auto relative = FPRelative(_current_begin->size(), false);
        _resolved[symbol] = relative;

        return relative;
    } else if (auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(symbol)) {
        // TODO: This is only for integer typed parameters
        if (parameter->index() < 6) {
            auto reg = INTEGER_ORDER[parameter->index()];
            _resolved[symbol] = reg;

            return reg;
        }

        _parameter_offset += 8;

        auto relative = FPRelative(_parameter_offset, true);
        _resolved[symbol] = relative;

        return relative;
    }

    throw std::invalid_argument("Only parameter and temporary symbols are resolvable.");
}
