#include "memory_resolver.hpp"

#include "instruction.hpp"
#include "utils.hpp"

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

void MemoryResolver::visit(IfNotInstruction &instruction) {
    instruction.set_condition(_resolve_operand(instruction.condition()));
}

void MemoryResolver::visit(EndInstruction &) {
    // Align the stack to comfort the specifications
    auto local_size = _current_begin->local_size();
    local_size = (local_size + STACK_ALIGNMENT - 1) & ~(STACK_ALIGNMENT - 1);
    _current_begin->set_local_size(local_size);
}

Operand MemoryResolver::_resolve_operand(const Operand &operand) {
    return std::visit(match{
        [&](const std::shared_ptr<Symbol> &value) -> Operand { return _resolve_symbol(value); },
        [&](const Immediate &value) -> Operand { return _resolve_immediate(value); },
        [](const auto &value) -> Operand { return value; }
    }, operand);
}

Operand MemoryResolver::_resolve_immediate(const Immediate &immediate) {
    if (!std::holds_alternative<float>(immediate) && !std::holds_alternative<double>(immediate)) return immediate;

    auto data_name = ".LC" + std::to_string(_data_index++);
    _data[data_name] = immediate;

    return Memory(immediate.size(), Memory::Address(data_name));
}

Operand MemoryResolver::_resolve_symbol(const std::shared_ptr<Symbol> &symbol) {
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) return result->second;

    Operand resolved = std::visit(match{
        [&](const TemporarySymbol &symbol) -> Operand { return _resolve_temporary(symbol); },
        [&](const ParameterSymbol &symbol) -> Operand { return _resolve_parameter(symbol); },
        [](const auto &) -> Operand { std::unreachable(); }
    }, *symbol);

    _resolved.emplace(symbol, resolved);
    return resolved;
}

Operand MemoryResolver::_resolve_temporary(const TemporarySymbol &symbol) {
    auto size = symbol.type().value().size();

    _current_begin->increase_local_size(_size_to_bytes(size));
    auto resolved = Memory(symbol.type().value().size(), RBP, -_current_begin->local_size());

    return resolved;
}

Operand MemoryResolver::_resolve_parameter(const ParameterSymbol &symbol) {
    auto &type = symbol.type().value();
    if (std::holds_alternative<IntegralType>(type) && symbol.int_index() < 6) {
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    } else if (std::holds_alternative<FloatingType>(type) && symbol.sse_index() < 8) {
        auto base = SSE_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    } else if (std::holds_alternative<BooleanType>(type) && symbol.int_index() < 6) {
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    }

    auto size = symbol.type().value().size();

    _parameter_offset += _size_to_bytes(size);
    auto resolved = Memory(size, RBP, _parameter_offset);

    return resolved;
}

std::optional<Register> MemoryResolver::_resolve_argument(const ParameterSymbol &symbol) {
    auto &type = symbol.type().value();
    if (std::holds_alternative<IntegralType>(type) && symbol.int_index() < 6) {
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    } else if (std::holds_alternative<FloatingType>(type) && symbol.sse_index() < 8) {
        auto base = SSE_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    } else if (std::holds_alternative<BooleanType>(type) && symbol.int_index() < 6) {
        auto base = INT_REG_ORDER[symbol.int_index()];
        return Register(base, type.size());
    }

    return std::nullopt;
}
