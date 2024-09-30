#include "gas_generator.h"

#include "instruction.h"
#include "il_printer.h"
#include "utils.h"

inline Register RBP(Register::Base::BP, Size::QWORD);
inline Register RSP(Register::Base::SP, Size::QWORD);
inline Register RDI(Register::Base::DI, Size::QWORD);
inline Register RAX(Register::Base::A, Size::QWORD);

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
                if (std::holds_alternative<IntegerType>(instruction.type()) &&
                    !std::holds_alternative<Memory>(instruction.right())) {
                    if (instruction.op() != BinaryInstruction::Operator::Div) return memory;
                }

                auto reg = _temp1_register(instruction.type());
                _mov(instruction.type(), reg, memory);
                return reg;
            },
            [&](const Immediate &immediate) -> Operand {
                auto reg = _temp1_register(instruction.type());
                _mov(instruction.type(), reg, immediate);
                return reg;
            },
            [](const auto &) -> Operand { throw std::invalid_argument("This operand is not supported."); },
    }, instruction.left());

    auto right_reg = std::visit(match{
            [](const Register &reg) -> Operand { return reg; },
            [](const Memory &memory) -> Operand { return memory; },
            [&](const Immediate &immediate) -> Operand {
                if (instruction.op() == BinaryInstruction::Operator::Div) {
                    auto temporary = _temp2_register(instruction.type());
                    _mov(instruction.type(), temporary, immediate);
                    return temporary;
                }

                return immediate;
            },
            [](const auto &) -> Operand { throw std::invalid_argument("This operand is not supported."); },
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

    if (auto *from_float = std::get_if<FloatingType>(&instruction.from())) {
        if (auto *to_float = std::get_if<FloatingType>(&instruction.to())) {
            _convert_float_to_float(*from_float, instruction.expression(), *to_float, instruction.result());
        } else if (auto *to_int = std::get_if<IntegerType>(&instruction.to())) {
            _convert_float_to_int(*from_float, instruction.expression(), *to_int, instruction.result());
        } else {
            throw std::runtime_error("This floating cast is not implemented yet.");
        }
    } else if (auto *from_int = std::get_if<IntegerType>(&instruction.from())) {
        if (auto *to_float = std::get_if<FloatingType>(&instruction.to())) {
            _convert_int_to_float(*from_int, instruction.expression(), *to_float, instruction.result());
        } else if (auto *to_int = std::get_if<IntegerType>(&instruction.to())) {
            _convert_int_to_int(*from_int, instruction.expression(), *to_int, instruction.result());
        } else {
            throw std::runtime_error("This integer cast is not implemented yet.");
        }
    } else {
        throw std::runtime_error("This cast is not implemented yet.");
    }

    _assembly.newline();
}

void GASGenerator::visit(CallInstruction &instruction) {
    _comment_instruction(instruction);

    _assembly.call(*instruction.symbol());

    auto &function = std::get<FunctionSymbol>(*instruction.symbol());
    auto return_reg = _returning_register(function.return_type());
    _mov(function.return_type(), instruction.result(), return_reg);

    _assembly.newline();
}

void GASGenerator::visit(ArgumentInstruction &instruction) {
    _comment_instruction(instruction);

    if (instruction.result()) {
        auto &parameter = std::get<ParameterSymbol>(*instruction.symbol());
        _mov(parameter.type(), *instruction.result(), instruction.expression());
    } else {
        _assembly.push(instruction.expression());
    }

    _assembly.newline();
}

void GASGenerator::visit(EndInstruction &instruction) {
    _comment_instruction(instruction);

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
    _assembly.label(TemporarySymbol("_start", {}));
    _assembly.call(TemporarySymbol("main", {}));
    _assembly.mov(RDI, RAX);
    _assembly.mov(RAX, Immediate(60));
    _assembly.syscall();

    _assembly.newline();
}

void GASGenerator::_data_section(const std::unordered_map<std::string, Immediate> &data) {
    _assembly.directive(".section", {".data"});
    for (const auto &[name, value]: data) {
        _assembly.label(TemporarySymbol(name, {}), false);

        if (std::holds_alternative<float>(value)) {
            _assembly.directive(".float", {to_string(value)});
        } else if (std::holds_alternative<double>(value)) {
            _assembly.directive(".double", {to_string(value)});
        } else {
            throw std::invalid_argument("This type is not implemented.");
        }
    }
}

void GASGenerator::_comment_instruction(Instruction &instruction) {
    auto printer = ILPrinter::print(instruction);
    _assembly.comment(printer.output().str());
}

void GASGenerator::_convert_int_to_int(const IntegerType &from, const Operand &expression,
                                       const IntegerType &to, const Operand &destination) {
    auto src = std::visit(match{
            [](const Register &reg) -> Operand { return reg; },
            [](const Memory &memory) -> Operand { return memory; },
            [&](const Immediate &immediate) -> Operand {
                auto temporary = _temp1_register(from);
                _assembly.mov(temporary, immediate);
                return temporary;
            },
            [](const auto &) -> Operand { throw std::runtime_error("This operand is not implemented."); }
    }, expression);

    Operand temporary = _temp1_register(to);
    if (from.size() == 32 && to.size() == 64) {
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

    _assembly.mov(destination, temporary);
}

void GASGenerator::_convert_float_to_float(const FloatingType &from, const Operand &expression,
                                           const FloatingType &to, const Operand &destination) {
    auto src = std::visit(match{
            [](const Register &reg) -> Operand { return reg; },
            [](const Memory &memory) -> Operand { return memory; },
            [](const auto &) -> Operand { throw std::runtime_error("This operand is not implemented."); }
    }, expression);

    if (from.size() == 32 && to.size() == 32) {
        _assembly.movss(destination, src);
    } else if (from.size() == 32 && to.size() == 64) {
        auto temporary = _temp1_register(to);
        _assembly.cvtss2sd(temporary, src);
        _assembly.movsd(destination, temporary);
    } else if (from.size() == 64 && to.size() == 32) {
        auto temporary = _temp1_register(to);
        _assembly.cvtsd2ss(temporary, src);
        _assembly.movss(destination, temporary);
    } else if (from.size() == 64 && to.size() == 64) {
        _assembly.movsd(destination, src);
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }
}

void GASGenerator::_convert_int_to_float(const IntegerType &from, const Operand &expression,
                                         const FloatingType &to, const Operand &destination) {
    auto src = (from.size() >= 32) ? expression : _integer_promote(from, expression);
    src = std::visit(match{
            [](const Register &reg) -> Operand { return reg; },
            [](const Memory &memory) -> Operand { return memory; },
            [&](const Immediate &value) -> Operand {
                auto temporary = _temp1_register(from);
                _assembly.mov(temporary, value);
                return temporary;
            },
            [](const auto &) -> Operand { throw std::runtime_error("This operand is not implemented."); }
    }, expression);

    auto temporary = _temp1_register(to);
    if (to.size() == 32) {
        _assembly.cvtsi2ss(temporary, src);
        _assembly.movss(destination, temporary);
    } else if (to.size() == 64) {
        _assembly.cvtsi2sd(temporary, src);
        _assembly.movsd(destination, temporary);
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }
}

void GASGenerator::_convert_float_to_int(const FloatingType &from, const Operand &expression,
                                         const IntegerType &to, const Operand &destination) {
    auto src = std::visit(match{
            [](const Register &reg) -> Operand { return reg; },
            [](const Memory &memory) -> Operand { return memory; },
            [](const auto &) -> Operand { throw std::runtime_error("This operand is not implemented."); }
    }, expression);

    auto bigger_to_temporary = _temp1_register(to);
    if (to.size() < 32) {
        auto temp_type = std::make_shared<IntegerType>(32, to.sign());
        bigger_to_temporary = _temp1_register(*temp_type);
    }

    if (from.size() == 32) {
        _assembly.cvttss2si(bigger_to_temporary, src);
    } else if (from.size() == 64) {
        _assembly.cvttsd2si(bigger_to_temporary, src);
    } else {
        throw std::runtime_error("This cast is not implemented.");
    }

    auto to_temporary = _temp1_register(to);
    _assembly.mov(destination, to_temporary);
}

Operand GASGenerator::_integer_promote(const IntegerType &type, const Operand &src) {
    if (type.size() >= 32) return src;

    auto new_type = std::make_shared<IntegerType>(32, type.sign());
    auto temporary = _temp1_register(*new_type);

    _assembly.movsx(temporary, src);

    return temporary;
}

void GASGenerator::_mov(const Type &type, const Operand &destination, const Operand &src) {
    if (std::holds_alternative<IntegerType>(type)) return _assembly.mov(destination, src);

    if (auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 64: return _assembly.movsd(destination, src);
            case 32: return _assembly.movss(destination, src);
            default: throw std::invalid_argument("Unsupported floating point size.");
        }
    }

    throw std::invalid_argument("This type is not implemented yet.");
}

void GASGenerator::_add(const Type &type, const Operand &destination, const Operand &src) {
    if (std::holds_alternative<IntegerType>(type)) return _assembly.add(destination, src);

    if (auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 64: return _assembly.addsd(destination, src);
            case 32: return _assembly.addss(destination, src);
            default: throw std::invalid_argument("Unsupported floating point size.");
        }
    }

    throw std::invalid_argument("This type is not implemented yet.");
}

void GASGenerator::_sub(const Type &type, const Operand &destination, const Operand &src) {
    if (std::holds_alternative<IntegerType>(type)) return _assembly.sub(destination, src);

    if (auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 64: return _assembly.subsd(destination, src);
            case 32: return _assembly.subss(destination, src);
            default: throw std::invalid_argument("Unsupported floating point size.");
        }
    }

    throw std::invalid_argument("This type is not implemented yet.");
}

void GASGenerator::_div(const Type &type, const Operand &destination, const Operand &src) {
    if (auto integer = std::get_if<IntegerType>(&type)) {
        if (integer->sign()) return _assembly.idiv(src);
        else return _assembly.div(src);
    }

    if (auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 64: return _assembly.divsd(destination, src);
            case 32: return _assembly.divss(destination, src);
            default: throw std::invalid_argument("Unsupported floating point size.");
        }
    }

    throw std::invalid_argument("This type is not implemented yet.");
}

void GASGenerator::_mul(const Type &type, const Operand &destination, const Operand &src) {
    if (std::get_if<IntegerType>(&type)) return _assembly.imul(destination, src);

    if (auto floating = std::get_if<FloatingType>(&type)) {
        switch (floating->size()) {
            case 64: return _assembly.mulsd(destination, src);
            case 32: return _assembly.mulss(destination, src);
            default: throw std::invalid_argument("Unsupported floating point size.");
        }
    }

    throw std::invalid_argument("This type is not implemented yet.");
}

Register GASGenerator::_select_register(const Type &type, Register::Base integer, Register::Base floating) {
    auto size = Register::type_to_register_size(type);

    if (std::holds_alternative<FloatingType>(type)) return {floating, size};
    if (std::holds_alternative<IntegerType>(type)) return {integer, size};

    throw std::invalid_argument("This type is not implemented.");
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
