#include "memory_resolver.h"

#include "instruction.h"

inline Register RBP(Register::Base::BP, Register::Size::QWORD);

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
    if (auto *symbolic = std::get_if<SymbolOperand>(&operand)) return _resolve_symbol(*symbolic->symbol());

    // If nothing has to be resolved, the operand is already finished.
    return operand;
}

Operand MemoryResolver::_resolve_symbol(const Symbol &symbol) {
    auto result = _resolved.find(&symbol);
    if (result != _resolved.end()) return result->second;

    if (auto *temporary = std::get_if<TemporarySymbol>(&symbol)) {
        auto location = _resolve_temporary(*temporary);
        _resolved.emplace(&symbol, location);
        return location;
    }

    if (auto *parameter = std::get_if<ParameterSymbol>(&symbol)) {
        auto location = _resolve_parameter(*parameter);
        _resolved.emplace(&symbol, location);
        return location;
    }

    throw std::invalid_argument("This symbol is not implemented.");
}

Operand MemoryResolver::_resolve_temporary(const TemporarySymbol &symbol) {
    auto byte_size = _type_to_byte_size(symbol.type());
    _current_begin->increase_local_size(byte_size);

    return Memory(RBP, -_current_begin->local_size());
}

Operand MemoryResolver::_resolve_parameter(const ParameterSymbol &symbol) {
    static const Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                    Register::Base::C, Register::Base::R8, Register::Base::R9};
    if (std::holds_alternative<IntegerType>(symbol.type()) && symbol.int_index() < 6) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    }

    static const Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                                    Register::Base::XMM2, Register::Base::XMM3,
                                                    Register::Base::XMM4, Register::Base::XMM5,
                                                    Register::Base::XMM6, Register::Base::XMM7};
    if (std::holds_alternative<FloatingType>(symbol.type()) && symbol.sse_index() < 8) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = SSE_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    }

    auto byte_size = _type_to_byte_size(symbol.type());
    _parameter_offset += byte_size;

    return Memory(RBP, _parameter_offset);
}

int64_t MemoryResolver::_type_to_byte_size(const Type &type) {
    if (auto *integer = std::get_if<IntegerType>(&type)) {
        switch (integer->size()) {
            case 8: return 1;
            case 16: return 2;
            case 32: return 4;
            case 64: return 8;
            default: throw std::invalid_argument("This integer size is not supported.");
        }
    } else if (auto *floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 32: return 4;
            case 64: return 8;
            default: throw std::invalid_argument("This floating point size is not supported.");
        }
    }

    throw std::invalid_argument("This type is not implemented.");
}
