#include "gas_generator.hpp"

#include "instruction.hpp"
#include "il_printer.hpp"
#include "utils.hpp"

inline Register RBP(Register::Base::BP, Size::QWORD);
inline Register RSP(Register::Base::SP, Size::QWORD);
inline Register RDI(Register::Base::DI, Size::QWORD);
inline Register RAX(Register::Base::A, Size::QWORD);

static BooleanType BOOL_TYPE;

GASGenerator GASGenerator::generate(const std::vector<std::unique_ptr<Instruction>> &instructions,
                                    const std::unordered_map<std::string, Immediate> &data) {
    GASGenerator generator;

    generator._preamble();

    for (const auto &item: instructions) {
        item->accept(generator);
    }

    generator._data_section(data);

    return generator;
}

void GASGenerator::visit(LabelInstruction &instruction) {
    _assembly.label(*instruction.symbol());
}

void GASGenerator::visit(BeginInstruction &instruction) {
    _comment_instruction(instruction);

    _assembly.label(*instruction.label());

    _assembly.push(RBP);
    _assembly.mov(RBP, RSP);

    if (instruction.local_size() != 0) _assembly.sub(RSP, Immediate(instruction.local_size()));

    _assembly.newline();
}

void GASGenerator::visit(ReturnInstruction &instruction) {
    _comment_instruction(instruction);

    auto destination = _returning_register(instruction.type());
    _mov(instruction.type(), destination, instruction.value());

    _assembly.newline();
}

void GASGenerator::visit(BinaryInstruction &instruction) {
    _comment_instruction(instruction);

    auto left_reg = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [&](const Memory &memory) -> Operand {
            if (std::holds_alternative<IntegralType>(instruction.type())
                && !std::holds_alternative<Memory>(instruction.right())
                && instruction.op() != BinaryInstruction::Operator::Div) {
                return memory;
            }

            return _move_to_temp1(instruction.type(), memory);
        },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(instruction.type(), immediate); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.left());

    auto right_reg = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &immediate) -> Operand {
            if (instruction.op() == BinaryInstruction::Operator::Div) {
                return _move_to_temp2(instruction.type(), immediate);
            }

            return immediate;
        },
        [](const std::shared_ptr<Symbol> &)-> Operand { std::unreachable(); }
    }, instruction.right());

    switch (instruction.op()) {
        case BinaryInstruction::Operator::Add: {
            _add(instruction.type(), left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Sub: {
            _sub(instruction.type(), left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Div: {
            _div(instruction.type(), left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Mul: {
            _mul(instruction.type(), left_reg, right_reg);
            break;
        }
    }

    _mov(instruction.type(), instruction.result(), left_reg);
    _assembly.newline();
}

void GASGenerator::visit(CastInstruction &instruction) {
    _comment_instruction(instruction);

    std::visit(match{
        [&](const FloatingType &from, const FloatingType &to) { _convert_float_to_float(instruction, from, to); },
        [&](const FloatingType &from, const IntegralType &to) { _convert_float_to_int(instruction, from, to); },
        [&](const FloatingType &from, const BooleanType &to) { _convert_float_to_bool(instruction, from, to); },
        [&](const IntegralType &from, const IntegralType &to) { _convert_int_to_int(instruction, from, to); },
        [&](const IntegralType &from, const FloatingType &to) { _convert_int_to_float(instruction, from, to); },
        [&](const IntegralType &from, const BooleanType &to) { _convert_int_to_bool(instruction, from, to); },
        [&](const BooleanType &from, const FloatingType &to) { _convert_bool_to_float(instruction, from, to); },
        [&](const BooleanType &from, const IntegralType &to) { _convert_bool_to_int(instruction, from, to); },
        [&](const BooleanType &, const BooleanType &) { throw std::runtime_error("Unsupported type conversion."); },
    }, instruction.from(), instruction.to());

    _assembly.newline();
}

void GASGenerator::visit(CallInstruction &instruction) {
    _comment_instruction(instruction);

    _assembly.call(*instruction.symbol());

    const auto &function = std::get<FunctionSymbol>(*instruction.symbol());
    const auto return_reg = _returning_register(function.return_type().value());
    _mov(function.return_type().value(), instruction.result(), return_reg);

    _assembly.newline();
}

void GASGenerator::visit(ArgumentInstruction &instruction) {
    _comment_instruction(instruction);

    if (instruction.result()) {
        const auto &parameter = std::get<ParameterSymbol>(*instruction.symbol());
        _mov(parameter.type().value(), *instruction.result(), instruction.expression());
    } else {
        _assembly.push(instruction.expression());
    }

    _assembly.newline();
}

void GASGenerator::visit(GotoInstruction &instruction) {
    _assembly.jmp(instruction.label());
}

void GASGenerator::visit(IfNotInstruction &instruction) {
    _comment_instruction(instruction);

    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(BOOL_TYPE, immediate); },
        [](const std::shared_ptr<Symbol> &)-> Operand { std::unreachable(); }
    }, instruction.condition());

    _assembly.cmp(src, Immediate((uint32_t) 0));
    _assembly.je(instruction.label());
}

void GASGenerator::visit(EndInstruction &instruction) {
    _comment_instruction(instruction);

    _assembly.label(*instruction.label());

    _assembly.mov(RSP, RBP);
    _assembly.pop(RBP);
    _assembly.ret();

    _assembly.newline();
}

void GASGenerator::_preamble() {
    _assembly.directive(".intel_syntax", {"noprefix"});
    _assembly.directive(".section", {".text"});
    _assembly.directive(".global", {"_start"});

    _assembly.newline();
    _assembly.label(TemporarySymbol("_start"));
    _assembly.call(TemporarySymbol("main"));
    _assembly.mov(RDI, RAX);
    _assembly.mov(RAX, Immediate(60));
    _assembly.syscall();

    _assembly.newline();
}

void GASGenerator::_data_section(const std::unordered_map<std::string, Immediate> &data) {
    _assembly.directive(".section", {".data"});
    for (const auto &[name, value]: data) {
        _assembly.label(TemporarySymbol(name), false);

        std::visit(match{
            [&](const float &) { _assembly.directive(".float", {to_string(value)}); },
            [&](const double &) { _assembly.directive(".double", {to_string(value)}); },
            [](const auto &) { throw std::runtime_error("This immediate is not implemented yet."); }
        }, value);
    }
}

void GASGenerator::_comment_instruction(Instruction &instruction) {
    auto printer = ILPrinter::print(instruction);
    _assembly.comment(printer.output().str());
}

void GASGenerator::_convert_int_to_int(const CastInstruction &instruction, const IntegralType &from,
                                       const IntegralType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [&](const Memory &memory) -> Operand {
            return std::visit(match{
                [&](const Memory &) -> Operand { return _move_to_temp1(from, memory); },
                [&](const auto &) -> Operand { return memory; },
            }, instruction.result());
        },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(from, immediate); },
        [](const std::shared_ptr<Symbol> &)-> Operand { std::unreachable(); }
    }, instruction.expression());

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

    _assembly.mov(instruction.result(), temporary);
}

void GASGenerator::_convert_int_to_float(const CastInstruction &instruction, const IntegralType &from,
                                         const FloatingType &to) {
    auto src = (from.size() >= Size::DWORD)
               ? instruction.expression()
               : _integer_promote(from, instruction.expression());
    src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &value) -> Operand { return _move_to_temp1(from, value); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, src);

    auto temporary = _temp1_register(to);
    switch (to.size()) {
        case Size::DWORD: {
            _assembly.cvtsi2ss(temporary, src);
            _assembly.movss(instruction.result(), temporary);
            break;
        }
        case Size::QWORD: {
            _assembly.cvtsi2sd(temporary, src);
            _assembly.movsd(instruction.result(), temporary);
            break;
        }
        default: throw std::runtime_error("This cast is not implemented.");
    }
}

void GASGenerator::_convert_int_to_bool(const CastInstruction &instruction, const IntegralType &from,
                                        const BooleanType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(from, immediate); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

    auto temporary = _temp1_register(to);
    _assembly.cmp(src, Immediate((uint32_t) 0));
    _assembly.setne(temporary);
    _assembly.mov(instruction.result(), temporary);
}

void GASGenerator::_convert_float_to_float(const CastInstruction &instruction, const FloatingType &from,
                                           const FloatingType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Immediate &) -> Operand { std::unreachable(); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

    if (from.size() == Size::DWORD && to.size() == Size::DWORD) {
        _assembly.movss(instruction.result(), src);
    } else if (from.size() == Size::DWORD && to.size() == Size::QWORD) {
        auto temporary = _temp1_register(to);
        _assembly.cvtss2sd(temporary, src);
        _assembly.movsd(instruction.result(), temporary);
    } else if (from.size() == Size::QWORD && to.size() == Size::DWORD) {
        auto temporary = _temp1_register(to);
        _assembly.cvtsd2ss(temporary, src);
        _assembly.movss(instruction.result(), temporary);
    } else if (from.size() == Size::QWORD && to.size() == Size::QWORD) {
        _assembly.movsd(instruction.result(), src);
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }
}

void GASGenerator::_convert_float_to_int(const CastInstruction &instruction, const FloatingType &from,
                                         const IntegralType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Immediate &) -> Operand { std::unreachable(); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

    auto bigger_to_temporary = _temp1_register(to);
    if (to.size() < Size::DWORD) {
        auto temp_type = std::make_shared<IntegralType>(Size::DWORD, to.sign());
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
    _assembly.mov(instruction.result(), to_temporary);
}

void GASGenerator::_convert_float_to_bool(const CastInstruction &instruction, const FloatingType &from,
                                          const BooleanType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [](const Immediate &) -> Operand { std::unreachable(); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

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
    _mov(to, instruction.result(), temporary);
}

void GASGenerator::_convert_bool_to_int(const CastInstruction &instruction, const BooleanType &from,
                                        const IntegralType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(from, immediate); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

    auto to_temporary = _temp1_register(to);
    _assembly.movzx(to_temporary, src);
    _assembly.mov(instruction.result(), to_temporary);
}

void GASGenerator::_convert_bool_to_float(const CastInstruction &instruction, const BooleanType &from,
                                          const FloatingType &to) {
    auto src = std::visit(match{
        [](const Register &reg) -> Operand { return reg; },
        [](const Memory &memory) -> Operand { return memory; },
        [&](const Immediate &immediate) -> Operand { return _move_to_temp1(from, immediate); },
        [](const std::shared_ptr<Symbol> &) -> Operand { std::unreachable(); }
    }, instruction.expression());

    auto to_int_temporary = _temp1_register(IntegralType(Size::DWORD, false));
    auto to_float_temporary = _temp1_register(to);

    _assembly.movzx(to_int_temporary, src);
    _assembly.pxor(to_float_temporary, to_float_temporary);

    switch (to.size()) {
        case Size::DWORD: {
            _assembly.cvtsi2ss(to_float_temporary, to_int_temporary);
            _assembly.movss(instruction.result(), to_float_temporary);
            break;
        }
        case Size::QWORD: {
            _assembly.cvtsi2sd(to_float_temporary, to_int_temporary);
            _assembly.movsd(instruction.result(), to_float_temporary);
            break;
        }
        default: std::unreachable();
    }
}

Operand GASGenerator::_integer_promote(const IntegralType &type, const Operand &src) {
    if (type.size() >= Size::DWORD) return src;

    auto new_type = std::make_shared<IntegralType>(Size::DWORD, type.sign());
    auto temporary = _temp1_register(*new_type);

    _assembly.movsx(temporary, src);

    return temporary;
}

void GASGenerator::_mov(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const IntegralType &) { _assembly.mov(destination, src); },
        [&](const BooleanType &) { _assembly.mov(destination, src); },
        [&](const FloatingType &type) {
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

void GASGenerator::_add(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const IntegralType &) { _assembly.add(destination, src); },
        [&](const BooleanType &) { _assembly.add(destination, src); },
        [&](const FloatingType &type) {
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

void GASGenerator::_sub(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const IntegralType &) { _assembly.sub(destination, src); },
        [&](const BooleanType &) { _assembly.sub(destination, src); },
        [&](const FloatingType &type) {
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

void GASGenerator::_div(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const IntegralType &type) {
            if (type.sign()) _assembly.idiv(src);
            else _assembly.div(src);
        },
        [&](const BooleanType &) { _assembly.div(src); },
        [&](const FloatingType &type) {
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

void GASGenerator::_mul(const Type &type, const Operand &destination, const Operand &src) {
    std::visit(match{
        [&](const IntegralType &) { _assembly.imul(destination, src); },
        [&](const BooleanType &) { _assembly.imul(destination, src); },
        [&](const FloatingType &type) {
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

Register GASGenerator::_move_to_temp1(const Type &type, const Operand &src) {
    auto reg = _temp1_register(type);
    _mov(type, reg, src);
    return reg;
}

Register GASGenerator::_move_to_temp2(const Type &type, const Operand &src) {
    auto reg = _temp2_register(type);
    _mov(type, reg, src);
    return reg;
}

Register GASGenerator::_select_register(const Type &type, Register::Base integer, Register::Base floating) {
    return std::visit(match{
        [&](const FloatingType &type) -> Register { return {floating, type.size()}; },
        [&](const IntegralType &type) -> Register { return {integer, type.size()}; },
        [&](const BooleanType &type) -> Register { return {integer, type.size()}; },
    }, type);
}

Register GASGenerator::_returning_register(const Type &type) {
    return _select_register(type, Register::Base::A, Register::Base::XMM0);
}

Register GASGenerator::_temp1_register(const Type &type) {
    return _select_register(type, Register::Base::A, Register::Base::XMM11);
}

Register GASGenerator::_temp2_register(const Type &type) {
    return _select_register(type, Register::Base::R11, Register::Base::XMM12);
}
