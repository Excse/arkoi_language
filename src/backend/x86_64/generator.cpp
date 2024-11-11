#include "backend/x86_64/generator.hpp"

#include <list>

#include "il/instruction.hpp"
#include "il/printer.hpp"
#include "utils/utils.hpp"

namespace x86_64 {

static const Register RBP(Register::Base::BP, Size::QWORD);
static const Register RSP(Register::Base::SP, Size::QWORD);
static const Register RDI(Register::Base::DI, Size::QWORD);
static const Register RAX(Register::Base::A, Size::QWORD);

static const type::Boolean BOOL_TYPE;

Generator Generator::generate(std::vector<Function> &functions) {
    Generator generator;

    generator._preamble();

    for (auto &function: functions) {
        generator._new_function(function);

        function.linearize([&](auto &instruction) {
            instruction.accept(generator);
        });
    }

    generator._data_section();

    return generator;
}

void Generator::visit(il::Label &instruction) {
    _assembly.label(instruction.symbol());
}

void Generator::visit(il::Begin &instruction) {
    _comment_instruction(instruction);

    _assembly.label(instruction.function());

    _assembly.push(RBP);
    _assembly.mov(RBP, RSP);

    if (_resolver.local_size() != 0) _assembly.sub(RSP, Constant(_resolver.local_size()));

    _assembly.newline();
}

void Generator::visit(il::Return &instruction) {
    _comment_instruction(instruction);

    const auto value = _resolver.resolve_operand(instruction.value());

    auto destination = _returning_register(instruction.type());
    _mov(instruction.type(), destination, value);

    _assembly.newline();
}

void Generator::visit(il::Binary &instruction) {
    _comment_instruction(instruction);

    const auto result = _resolver.resolve_operand(instruction.result());

    const auto left = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [&](const Memory &memory) -> Operand { return _move_to_temp1(instruction.type(), memory); },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(instruction.type(), constant); },
    }, _resolver.resolve_operand(instruction.left()));

    const auto right = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand {
            if (instruction.op() == il::Binary::Operator::Div) {
                return _move_to_temp2(instruction.type(), constant);
            } else {
                return constant;
            }
        },
    }, _resolver.resolve_operand(instruction.right()));

    switch (instruction.op()) {
        case il::Binary::Operator::Add: {
            _add(instruction.type(), left, right);
            break;
        }
        case il::Binary::Operator::Sub: {
            _sub(instruction.type(), left, right);
            break;
        }
        case il::Binary::Operator::Div: {
            _div(instruction.type(), left, right);
            break;
        }
        case il::Binary::Operator::Mul: {
            _mul(instruction.type(), left, right);
            break;
        }
    }

    _mov(instruction.type(), result, left);
    _assembly.newline();
}

void Generator::visit(il::Cast &instruction) {
    _comment_instruction(instruction);

    std::visit(match{
        [&](const type::Floating &from, const type::Floating &to) { _convert_float_to_float(instruction, from, to); },
        [&](const type::Floating &from, const type::Integral &to) { _convert_float_to_int(instruction, from, to); },
        [&](const type::Floating &from, const type::Boolean &to) { _convert_float_to_bool(instruction, from, to); },
        [&](const type::Integral &from, const type::Integral &to) { _convert_int_to_int(instruction, from, to); },
        [&](const type::Integral &from, const type::Floating &to) { _convert_int_to_float(instruction, from, to); },
        [&](const type::Integral &from, const type::Boolean &to) { _convert_int_to_bool(instruction, from, to); },
        [&](const type::Boolean &from, const type::Floating &to) { _convert_bool_to_float(instruction, from, to); },
        [&](const type::Boolean &from, const type::Integral &to) { _convert_bool_to_int(instruction, from, to); },
        [&](const type::Boolean &, const type::Boolean &) { throw std::runtime_error("Unsupported type conversion."); },
    }, instruction.from(), instruction.to());

    _assembly.newline();
}

void Generator::visit(il::Call &instruction) {
    _comment_instruction(instruction);

    std::list<Operand> stack_push;

    const auto &function = std::get<symbol::Function>(*instruction.function());
    for (size_t index = 0; index < function.parameter_symbols().size(); index++) {
        const auto &parameter_symbol = function.parameter_symbols()[index];
        const auto &parameter = std::get<symbol::Parameter>(*parameter_symbol);

        const auto &argument = instruction.arguments()[index];

        const auto expression = _resolver.resolve_operand(argument);
        auto result = _resolver.resolve_operand(parameter_symbol);

        if (std::holds_alternative<Register>(result)) {
            _mov(parameter.type().value(), result, expression);
        } else {
            stack_push.push_front(expression);
        }
    }

    for (const auto &item: stack_push) {
        _assembly.push(item);
    }

    const auto result = _resolver.resolve_operand(instruction.result());

    _assembly.call(instruction.function());

    const auto return_reg = _returning_register(function.return_type().value());
    _mov(function.return_type().value(), result, return_reg);

    _assembly.newline();
}

void Generator::visit(il::Goto &instruction) {
    _assembly.jmp(instruction.label());
}

void Generator::visit(il::If &instruction) {
    _comment_instruction(instruction);

    const auto condition = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(BOOL_TYPE, constant); },
    }, _resolver.resolve_operand(instruction.condition()));

    _assembly.cmp(condition, Constant((uint32_t) 0));
    _assembly.jne(instruction.label());
}

void Generator::visit(il::Store &instruction) {
    _comment_instruction(instruction);

    const auto result = _resolver.resolve_operand(instruction.result());

    const auto value = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [&](const Memory &memory) -> Operand {
            if (!std::holds_alternative<Memory>(result)) return memory;

            return _move_to_temp1(instruction.type(), memory);
        },
        [&](const Constant &constant) -> Operand { return constant; },
    }, _resolver.resolve_operand(instruction.value()));

    _mov(instruction.type(), result, value);

    _assembly.newline();
}

void Generator::visit(il::End &instruction) {
    _comment_instruction(instruction);

    _assembly.mov(RSP, RBP);
    _assembly.pop(RBP);
    _assembly.ret();

    _assembly.newline();
}

void Generator::_new_function(Function &function) {
    _resolver = OperandResolver::resolve(function);
    for (const auto &[constant, data]: _resolver.constants()) {
        _constants.emplace(constant, data.name);
    }
}

void Generator::_preamble() {
    _assembly.directive(".intel_syntax", {"noprefix"});
    _assembly.directive(".section", {".text"});
    _assembly.directive(".global", {"_start"});

    _assembly.newline();
    _assembly.label("_start");
    _assembly.call("main");
    _assembly.mov(RDI, RAX);
    _assembly.mov(RAX, Constant(60));
    _assembly.syscall();

    _assembly.newline();
}

void Generator::_data_section() {
    _assembly.directive(".section", {".data"});
    for (const auto &[value, name]: _constants) {
        _assembly.label(name, false);

        std::visit(match{
            [&](const float &) { _assembly.directive(".float", {to_string(value)}); },
            [&](const double &) { _assembly.directive(".double", {to_string(value)}); },
            [](const auto &) { throw std::runtime_error("This immediate is not implemented yet."); }
        }, value);
    }
}

void Generator::_comment_instruction(il::Instruction &instruction) {
    auto printer = il::Printer::print(instruction);
    _assembly.comment(printer.output().str());
}

void Generator::_convert_int_to_int(const il::Cast &instruction, const type::Integral &from,
                                    const type::Integral &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [&](const Memory &memory) -> Operand {
            return std::visit(match{
                [&](const Memory &) -> Operand { return _move_to_temp1(from, memory); },
                [&](const Constant &) -> Operand { return memory; },
                [&](const Register &) -> Operand { return memory; },
            }, result);
        },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(from, constant); },
    }, _resolver.resolve_operand(instruction.expression()));

    Operand temporary = _temp1_register(to);
    if (from.size() == Size::DWORD && to.size() == Size::QWORD) {
        _assembly.movsxd(temporary, src);
    } else if (to.size() > from.size()) {
        _assembly.movsx(temporary, src);
    } else if (to.size() < from.size()) {
        // Nothing to do here, temporary is already a register that is smaller
    } else if (to.size() == from.size()) {
        temporary = src;
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }

    _assembly.mov(result, temporary);
}

void Generator::_convert_int_to_float(const il::Cast &instruction, const type::Integral &from,
                                      const type::Floating &to) {
    auto expression = _resolver.resolve_operand(instruction.expression());
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = (from.size() >= Size::DWORD) ? expression : _integer_promote(from, expression);
    src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(from, constant); },
    }, src);

    auto temporary = _temp1_register(to);
    switch (to.size()) {
        case Size::DWORD: {
            _assembly.cvtsi2ss(temporary, src);
            _assembly.movss(result, temporary);
            break;
        }
        case Size::QWORD: {
            _assembly.cvtsi2sd(temporary, src);
            _assembly.movsd(result, temporary);
            break;
        }
        default: throw std::runtime_error("This cast is not implemented.");
    }
}

void Generator::_convert_int_to_bool(const il::Cast &instruction, const type::Integral &from,
                                     const type::Boolean &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(from, constant); },
    }, _resolver.resolve_operand(instruction.expression()));

    auto temporary = _temp1_register(to);
    _assembly.cmp(src, Constant((uint32_t) 0));
    _assembly.setne(temporary);
    _assembly.mov(result, temporary);
}

void Generator::_convert_float_to_float(const il::Cast &instruction, const type::Floating &from,
                                        const type::Floating &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Constant &) -> Operand { std::unreachable(); },
    }, _resolver.resolve_operand(instruction.expression()));

    if (from.size() == Size::DWORD && to.size() == Size::DWORD) {
        _assembly.movss(result, src);
    } else if (from.size() == Size::DWORD && to.size() == Size::QWORD) {
        auto temporary = _temp1_register(to);
        _assembly.cvtss2sd(temporary, src);
        _assembly.movsd(result, temporary);
    } else if (from.size() == Size::QWORD && to.size() == Size::DWORD) {
        auto temporary = _temp1_register(to);
        _assembly.cvtsd2ss(temporary, src);
        _assembly.movss(result, temporary);
    } else if (from.size() == Size::QWORD && to.size() == Size::QWORD) {
        _assembly.movsd(result, src);
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }
}

void Generator::_convert_float_to_int(const il::Cast &instruction, const type::Floating &from,
                                      const type::Integral &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Constant &) -> Operand { std::unreachable(); },
    }, _resolver.resolve_operand(instruction.expression()));

    auto bigger_to_temporary = _temp1_register(to);
    if (to.size() < Size::DWORD) {
        auto temp_type = std::make_shared<type::Integral>(Size::DWORD, to.sign());
        bigger_to_temporary = _temp1_register(*temp_type);
    }

    switch (from.size()) {
        case Size::DWORD: {
            _assembly.cvttss2si(bigger_to_temporary, src);
            break;
        }
        case Size::QWORD: {
            _assembly.cvttsd2si(bigger_to_temporary, src);
            break;
        }
        default: std::unreachable();
    }

    auto to_temporary = _temp1_register(to);
    _assembly.mov(result, to_temporary);
}

void Generator::_convert_float_to_bool(const il::Cast &instruction, const type::Floating &from,
                                       const type::Boolean &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Constant &) -> Operand { std::unreachable(); },
    }, _resolver.resolve_operand(instruction.expression()));

    auto zero = _temp2_register(from);
    _assembly.pxor(zero, zero);

    switch (from.size()) {
        case Size::DWORD: {
            _assembly.ucomiss(zero, src);
            break;
        }
        case Size::QWORD: {
            _assembly.ucomisd(zero, src);
            break;
        }
        default: std::unreachable();
    }

    auto temporary = _temp1_register(to);
    _assembly.setne(temporary);
    _mov(to, result, temporary);
}

void Generator::_convert_bool_to_int(const il::Cast &instruction, const type::Boolean &from,
                                     const type::Integral &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(from, constant); },
    }, _resolver.resolve_operand(instruction.expression()));

    auto to_temporary = _temp1_register(to);
    _assembly.movzx(to_temporary, src);
    _assembly.mov(result, to_temporary);
}

void Generator::_convert_bool_to_float(const il::Cast &instruction, const type::Boolean &from,
                                       const type::Floating &to) {
    auto result = _resolver.resolve_operand(instruction.result());

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Constant &constant) -> Operand { return _move_to_temp1(from, constant); },
    }, _resolver.resolve_operand(instruction.expression()));

    auto to_int_temporary = _temp1_register(type::Integral(Size::DWORD, false));
    auto to_float_temporary = _temp1_register(to);

    _assembly.movzx(to_int_temporary, src);
    _assembly.pxor(to_float_temporary, to_float_temporary);

    switch (to.size()) {
        case Size::DWORD: {
            _assembly.cvtsi2ss(to_float_temporary, to_int_temporary);
            _assembly.movss(result, to_float_temporary);
            break;
        }
        case Size::QWORD: {
            _assembly.cvtsi2sd(to_float_temporary, to_int_temporary);
            _assembly.movsd(result, to_float_temporary);
            break;
        }
        default: std::unreachable();
    }
}

Operand Generator::_integer_promote(const type::Integral &type, const Operand &src) {
    if (type.size() >= Size::DWORD) return src;

    auto new_type = std::make_shared<type::Integral>(Size::DWORD, type.sign());
    auto temporary = _temp1_register(*new_type);

    _assembly.movsx(temporary, src);

    return temporary;
}

void Generator::_mov(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const type::Integral &) { _assembly.mov(destination, src); },
        [&](const type::Boolean &) { _assembly.mov(destination, src); },
        [&](const type::Floating &type) {
            switch (type.size()) {
                case Size::QWORD: {
                    _assembly.movsd(destination, src);
                    break;
                }
                case Size::DWORD: {
                    _assembly.movss(destination, src);
                    break;
                }
                default: std::unreachable();
            }
        }
    }, type);
}

void Generator::_add(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const type::Integral &) { _assembly.add(destination, src); },
        [&](const type::Boolean &) { _assembly.add(destination, src); },
        [&](const type::Floating &type) {
            switch (type.size()) {
                case Size::QWORD: {
                    _assembly.addsd(destination, src);
                    break;
                }
                case Size::DWORD: {
                    _assembly.addss(destination, src);
                    break;
                }
                default: std::unreachable();
            }
        }
    }, type);
}

void Generator::_sub(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const type::Integral &) { _assembly.sub(destination, src); },
        [&](const type::Boolean &) { _assembly.sub(destination, src); },
        [&](const type::Floating &type) {
            switch (type.size()) {
                case Size::QWORD: {
                    _assembly.subsd(destination, src);
                    break;
                }
                case Size::DWORD: {
                    _assembly.subss(destination, src);
                    break;
                }
                default: std::unreachable();
            }
        }
    }, type);
}

void Generator::_div(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const type::Integral &type) {
            if (type.sign()) _assembly.idiv(src);
            else _assembly.div(src);
        },
        [&](const type::Boolean &) { _assembly.div(src); },
        [&](const type::Floating &type) {
            switch (type.size()) {
                case Size::QWORD: {
                    _assembly.divsd(destination, src);
                    break;
                }
                case Size::DWORD: {
                    _assembly.divss(destination, src);
                    break;
                }
                default: std::unreachable();
            }
        }
    }, type);
}

void Generator::_mul(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const type::Integral &) { _assembly.imul(destination, src); },
        [&](const type::Boolean &) { _assembly.imul(destination, src); },
        [&](const type::Floating &type) {
            switch (type.size()) {
                case Size::QWORD: {
                    _assembly.mulsd(destination, src);
                    break;
                }
                case Size::DWORD: {
                    _assembly.mulss(destination, src);
                    break;
                }
                default: std::unreachable();
            }
        }
    }, type);
}

Register Generator::_move_to_temp1(const Type &type, const Operand &src) {
    auto reg = _temp1_register(type);
    _mov(type, reg, src);
    return reg;
}

Register Generator::_move_to_temp2(const Type &type, const Operand &src) {
    auto reg = _temp2_register(type);
    _mov(type, reg, src);
    return reg;
}

Register Generator::_select_register(const Type &type, Register::Base integer, Register::Base floating) {
    return std::visit(match{
        [&](const type::Floating &type) -> Register { return {floating, type.size()}; },
        [&](const type::Integral &type) -> Register { return {integer, type.size()}; },
        [&](const type::Boolean &type) -> Register { return {integer, type.size()}; },
    }, type);
}

Register Generator::_returning_register(const Type &type) {
    return _select_register(type, Register::Base::A, Register::Base::XMM0);
}

Register Generator::_temp1_register(const Type &type) {
    return _select_register(type, Register::Base::A, Register::Base::XMM11);
}

Register Generator::_temp2_register(const Type &type) {
    return _select_register(type, Register::Base::R11, Register::Base::XMM12);
}

}
