#include "memory_resolver.h"

#include "instruction.h"

static const Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                Register::Base::C, Register::Base::R8, Register::Base::R9};
static const Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                                Register::Base::XMM2, Register::Base::XMM3,
                                                Register::Base::XMM4, Register::Base::XMM5,
                                                Register::Base::XMM6, Register::Base::XMM7};

inline Register RBP(Register::Base::BP, Size::QWORD);

inline int64_t STACK_ALIGNMENT = 16;

MemoryResolver MemoryResolver::resolve(const std::vector<std::unique_ptr<Instruction>> &instructions) {
    MemoryResolver resolver;

    for (const auto &item: instructions) {
        item->accept(resolver);
    }

    return resolver;
}

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

void MemoryResolver::visit(ArgumentInstruction &instruction) {
    auto &parameter = std::get<ParameterSymbol>(*instruction.symbol());

    auto reg = _resolve_argument(parameter);
    if (reg.has_value()) instruction.set_result(reg.value());

    instruction.set_expression(_resolve_operand(instruction.expression()));
}

void MemoryResolver::visit(CallInstruction &instruction) {
    instruction.set_result(_resolve_operand(instruction.result()));
}

void MemoryResolver::visit(EndInstruction &) {
    // Align the stack to comfort the specifications
    auto local_size = _current_begin->local_size();
    local_size = (local_size + STACK_ALIGNMENT - 1) & ~(STACK_ALIGNMENT - 1);
    _current_begin->set_local_size(local_size);
}

Operand MemoryResolver::_resolve_operand(const Operand &operand) {
    if (auto *symbolic = std::get_if<std::shared_ptr<Symbol>>(&operand)) return _resolve_symbol(*symbolic);
    if (auto *immediate = std::get_if<Immediate>(&operand)) return _resolve_immediate(*immediate);

    // If nothing has to be resolved, the operand is already finished.
    return operand;
}

Operand MemoryResolver::_resolve_immediate(const Immediate &immediate) {
    if (!std::holds_alternative<float>(immediate) && !std::holds_alternative<double>(immediate)) return immediate;

    auto data_name = ".LC" + std::to_string(_data_index++);
    _data[data_name] = immediate;

    if (std::holds_alternative<float>(immediate)) {
        return Memory(Size::DWORD, Memory::Address(data_name));
    } else if (std::holds_alternative<double>(immediate)) {
        return Memory(Size::QWORD, Memory::Address(data_name));
    } else {
        throw std::invalid_argument("This type is not implemented.");
    }
}

Operand MemoryResolver::_resolve_symbol(const std::shared_ptr<Symbol> &symbol) {
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) return result->second;

    if (auto *temporary = std::get_if<TemporarySymbol>(symbol.get())) {
        auto location = _resolve_temporary(*temporary);
        _resolved.emplace(symbol, location);
        return location;
    }

    if (auto *parameter = std::get_if<ParameterSymbol>(symbol.get())) {
        auto location = _resolve_parameter(*parameter);
        _resolved.emplace(symbol, location);
        return location;
    }

    throw std::invalid_argument("This symbol is not implemented.");
}

Operand MemoryResolver::_resolve_temporary(const TemporarySymbol &symbol) {
    auto byte_size = _type_to_byte_size(symbol.type());
    _current_begin->increase_local_size(byte_size);

    auto size = _byte_size_to_size(byte_size);
    return Memory(size, RBP, -_current_begin->local_size());
}

Operand MemoryResolver::_resolve_parameter(const ParameterSymbol &symbol) {
    if (std::holds_alternative<IntegralType>(symbol.type()) && symbol.int_index() < 6) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    } else if (std::holds_alternative<FloatingType>(symbol.type()) && symbol.sse_index() < 8) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = SSE_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    } else if (std::holds_alternative<BooleanType>(symbol.type()) && symbol.int_index() < 6) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    }

    auto byte_size = _type_to_byte_size(symbol.type());
    _parameter_offset += byte_size;

    auto size = _byte_size_to_size(byte_size);
    return Memory(size, RBP, _parameter_offset);
}

std::optional<Register> MemoryResolver::_resolve_argument(const ParameterSymbol &symbol) {
    if (std::holds_alternative<IntegralType>(symbol.type()) && symbol.int_index() < 6) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    } else if (std::holds_alternative<FloatingType>(symbol.type()) && symbol.sse_index() < 8) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = SSE_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    } else if (std::holds_alternative<BooleanType>(symbol.type()) && symbol.int_index() < 6) {
        auto size = Register::type_to_register_size(symbol.type());
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, size);
    }

    return std::nullopt;
}

int64_t MemoryResolver::_type_to_byte_size(const Type &type) {
    if (auto *integer = std::get_if<IntegralType>(&type)) {
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
    } else if (std::get_if<BooleanType>(&type)) {
        return 1;
    }

    throw std::invalid_argument("This type is not implemented.");
}

Size MemoryResolver::_byte_size_to_size(int64_t bytes) {
    switch (bytes) {
        case 1: return Size::BYTE;
        case 2: return Size::WORD;
        case 4: return Size::DWORD;
        case 8: return Size::QWORD;
        default: throw std::invalid_argument("This byte size is not implemented.");
    }
}
