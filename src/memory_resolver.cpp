#include "memory_resolver.h"

#include "instruction.h"

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

void MemoryResolver::visit(CastInstruction &instruction) {
    instruction.set_result(_resolve_operand(instruction.result()));
    instruction.set_expression(_resolve_operand(instruction.expression()));
}

Operand MemoryResolver::_resolve_operand(const Operand &operand) {
    if (!std::holds_alternative<std::shared_ptr<Symbol>>(operand.data())) {
        return operand;
    }

    auto symbol = std::get<std::shared_ptr<Symbol>>(operand.data());
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) {
        return result->second;
    }

    if (auto temporary = std::dynamic_pointer_cast<TemporarySymbol>(symbol)) {
        _current_begin->increase_local_size(8);

        auto location = Operand(Memory(Register::RBP, -_current_begin->local_size()));
        _resolved[symbol] = location;

        return location;
    } else if (auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(symbol)) {
        static const Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                        Register::Base::C, Register::Base::R8, Register::Base::R9};

        // TODO: This is only for integer typed parameters
        if (parameter->index() < 6) {
            auto size = Register::type_to_register_size(parameter->type());
            auto base = INT_REG_ORDER[parameter->index()];
            auto reg = Operand(Register(base, size));
            _resolved[symbol] = reg;

            return reg;
        }

        _parameter_offset += 8;

        auto location = Operand(Memory(Register::RBP, _parameter_offset));
        _resolved[symbol] = location;

        return location;
    }

    throw std::invalid_argument("Only parameter and temporary symbols are resolvable.");
}
