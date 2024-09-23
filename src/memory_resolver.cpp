#include "memory_resolver.h"

#include "instruction.h"

static const auto RBP = Register(Register::Base::BP, Register::Size::QWORD);

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

std::shared_ptr<Operand> MemoryResolver::_resolve_operand(const std::shared_ptr<Operand> &operand) {
    auto symbolic = std::dynamic_pointer_cast<SymbolOperand>(operand);
    if (!symbolic) {
        return operand;
    }

    auto symbol = symbolic->symbol();
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) {
        return result->second;
    }

    if (auto temporary = std::dynamic_pointer_cast<TemporarySymbol>(symbol)) {
        auto byte_size = _type_to_byte_size(temporary->type());
        _current_begin->increase_local_size(byte_size);

        auto location = std::make_shared<Memory>(RBP, -_current_begin->local_size());
        _resolved[symbol] = location;

        return location;
    } else if (auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(symbol)) {
        static const Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                        Register::Base::C, Register::Base::R8, Register::Base::R9};
        if (std::dynamic_pointer_cast<IntegerType>(parameter->type()) && parameter->int_index() < 6) {
            auto size = Register::type_to_register_size(*parameter->type());
            auto base = INT_REG_ORDER[parameter->int_index()];
            auto reg = std::make_shared<Register>(base, size);
            _resolved[symbol] = reg;
            return reg;
        }

        static const Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                                        Register::Base::XMM2, Register::Base::XMM3,
                                                        Register::Base::XMM4, Register::Base::XMM5,
                                                        Register::Base::XMM6, Register::Base::XMM7};
        if (std::dynamic_pointer_cast<FloatingType>(parameter->type()) && parameter->sse_index() < 8) {
            auto size = Register::type_to_register_size(*parameter->type());
            auto base = SSE_REG_ORDER[parameter->int_index()];
            auto reg = std::make_shared<Register>(base, size);
            _resolved[symbol] = reg;
            return reg;
        }

        auto byte_size = _type_to_byte_size(parameter->type());
        _parameter_offset += byte_size;

        auto location = std::make_shared<Memory>(RBP, _parameter_offset);
        _resolved[symbol] = location;

        return location;
    }

    throw std::invalid_argument("Only parameter and temporary symbols are resolvable.");
}

int64_t MemoryResolver::_type_to_byte_size(const std::shared_ptr<Type> &type) {
    if (auto integer = std::dynamic_pointer_cast<IntegerType>(type)) {
        switch (integer->size()) {
            case 8: return 1;
            case 16: return 2;
            case 32: return 4;
            case 64: return 8;
        }
    } else if (auto floating = std::dynamic_pointer_cast<FloatingType>(type)) {
        switch (floating->size()) {
            case 32: return 4;
            case 64: return 8;
        }
    }

    throw std::runtime_error("This type is not implemented.");
}
