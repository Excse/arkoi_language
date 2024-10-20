#include "backend/x86_64/operand_resolver.hpp"

#include "il/instruction.hpp"
#include "utils/utils.hpp"

namespace x86_64 {

static const Register::Base INT_REG_ORDER[6] = {Register::Base::DI, Register::Base::SI, Register::Base::D,
                                                Register::Base::C, Register::Base::R8, Register::Base::R9};

static const Register::Base SSE_REG_ORDER[8] = {Register::Base::XMM0, Register::Base::XMM1,
                                                Register::Base::XMM2, Register::Base::XMM3,
                                                Register::Base::XMM4, Register::Base::XMM5,
                                                Register::Base::XMM6, Register::Base::XMM7};

static const Register RBP(Register::Base::BP, Size::QWORD);

OperandResolver OperandResolver::resolve(CFG &cfg) {
    OperandResolver resolver;

    cfg.linearize([&](auto &instruction) {
        instruction.accept(resolver);
    });

    return resolver;
}

void OperandResolver::visit(il::Begin &instruction) {
    const auto &function = std::get<FunctionSymbol>(*instruction.label());

    // This is necessary for the correct argument passing using the Int/SSE Registers of the calling convention
    for (auto &symbol: function.parameter_symbols()) {
        const auto &parameter_symbol = std::get<ParameterSymbol>(*symbol);
        std::ignore = _resolve_parameter(parameter_symbol);
    }
}

void OperandResolver::visit(il::Return &instruction) {
    std::ignore = resolve_operand(instruction.value());
}

void OperandResolver::visit(il::Binary &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.left());
    std::ignore = resolve_operand(instruction.right());
}

void OperandResolver::visit(il::Cast &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.expression());
}

void OperandResolver::visit(il::Argument &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.expression());
}

void OperandResolver::visit(il::Call &instruction) {
    std::ignore = resolve_operand(instruction.result());
}

void OperandResolver::visit(il::IfNot &instruction) {
    std::ignore = resolve_operand(instruction.condition());
}

void OperandResolver::visit(il::Store &instruction) {
    std::ignore = resolve_operand(instruction.result());
    std::ignore = resolve_operand(instruction.value());
}

void OperandResolver::visit(il::End &) {
    static int64_t STACK_ALIGNMENT = 16;
    // Align the stack to comfort the specifications
    _local_size = (_local_size + STACK_ALIGNMENT - 1) & ~(STACK_ALIGNMENT - 1);
}

x86_64::Operand OperandResolver::resolve_operand(const il::Operand &operand) {
    return std::visit(match{
        [&](const Symbol &value) -> x86_64::Operand { return _resolve_symbol(value); },
        [&](const Constant &value) -> x86_64::Operand { return _resolve_constant(value); },
    }, operand);
}

x86_64::Operand OperandResolver::_resolve_constant(const il::Constant &constant) {
    if (!std::holds_alternative<float>(constant) && !std::holds_alternative<double>(constant)) return constant;

    auto result = _constants.find(constant);
    if (result != _constants.end()) return result->second.operand;

    auto data_name = ".LC" + std::to_string(_constant_index++);
    auto resolved = Memory(constant.size(), Memory::Address(data_name));

    _constants.emplace(constant, ConstantData{resolved, data_name});
    return resolved;
}

x86_64::Operand OperandResolver::_resolve_symbol(const Symbol &symbol) {
    auto result = _resolved.find(symbol);
    if (result != _resolved.end()) return result->second;

    x86_64::Operand resolved = std::visit(match{
        [&](const TemporarySymbol &symbol) -> x86_64::Operand { return _resolve_temporary(symbol); },
        [&](const ParameterSymbol &symbol) -> x86_64::Operand { return _resolve_parameter(symbol); },
        [](const FunctionSymbol &) -> x86_64::Operand { std::unreachable(); }
    }, *symbol);

    _resolved.emplace(symbol, resolved);
    return resolved;
}

x86_64::Operand OperandResolver::_resolve_temporary(const TemporarySymbol &symbol) {
    auto size = symbol.type().value().size();

    _local_size += (int64_t) size_to_bytes(size);
    auto resolved = Memory(symbol.type().value().size(), RBP, -_local_size);

    return resolved;
}

x86_64::Operand OperandResolver::_resolve_parameter(const ParameterSymbol &symbol) {
    auto resolved_register = _resolve_parameter_register(symbol);
    if (resolved_register) return *resolved_register;

    auto size = symbol.type().value().size();

    _parameter_offset += (int64_t) size_to_bytes(size);
    auto resolved = Memory(size, RBP, _parameter_offset);

    return resolved;
}

std::optional<Register> OperandResolver::_resolve_parameter_register(const ParameterSymbol &symbol) {
    return std::visit(match{
        [&](const type::Integral &type) -> std::optional<Register> {
            if (_int_index >= 6) return std::nullopt;

            return Register(INT_REG_ORDER[_int_index++], type.size());
        },
        [&](const type::Boolean &) -> std::optional<Register> {
            if (_int_index >= 6) return std::nullopt;

            return Register(INT_REG_ORDER[_int_index], type::Boolean::size());
        },
        [&](const type::Floating &type) -> std::optional<Register> {
            if (_sse_index >= 8) return std::nullopt;

            return Register(SSE_REG_ORDER[_sse_index++], type.size());
        },
    }, symbol.type().value());
}

}