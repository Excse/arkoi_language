#include "gas_generator.h"

#include "instruction.h"
#include "il_printer.h"

inline Register RBP(Register::Base::BP, Size::QWORD);
inline Register RSP(Register::Base::SP, Size::QWORD);

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
    _assembly.newline();
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

    auto left_reg = _temp1_register(instruction.type());
    _mov(instruction.type(), left_reg, instruction.left());

    auto right_reg = _temp2_register(instruction.type());
    _mov(instruction.type(), right_reg, instruction.right());

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

    const auto &expression = instruction.expression();
    const auto &destination = instruction.result();
    const auto &from = instruction.from();
    const auto &to = instruction.to();

    if (auto *from_float = std::get_if<FloatingType>(&from)) {
        if (auto *to_float = std::get_if<FloatingType>(&to)) {
            _convert_float_to_float(*from_float, expression, *to_float, destination);
            _assembly.newline();
            return;
        }

        if (auto *to_int = std::get_if<IntegerType>(&to)) {
            _convert_float_to_int(*from_float, expression, *to_int, destination);
            _assembly.newline();
            return;
        }

        throw std::runtime_error("This floating cast is not implemented yet.");
    } else if (auto *from_int = std::get_if<IntegerType>(&from)) {
        if (auto *to_float = std::get_if<FloatingType>(&to)) {
            _convert_int_to_float(*from_int, expression, *to_float, destination);
            _assembly.newline();
            return;
        } else if (auto *to_int = std::get_if<IntegerType>(&to)) {
            _convert_int_to_int(*from_int, expression, *to_int, destination);
            _assembly.newline();
            return;
        }

        throw std::runtime_error("This integer cast is not implemented yet.");
    }

    throw std::runtime_error("This cast is not implemented yet.");
}

void GASGenerator::visit(EndInstruction &instruction) {
    _comment_instruction(instruction);

    _assembly.mov(RSP, RBP);
    _assembly.pop(RBP);
    _assembly.ret();

    _assembly.newline();
}

void GASGenerator::_preamble() {
    _assembly.output() << R"(
.intel_syntax noprefix
.section .text
.global _start

_start:
    movsd xmm0, QWORD PTR [test]
    call main

    mov rdi, rax
    mov rax, 60
    syscall
)";
}

void GASGenerator::_data_section(const std::unordered_map<std::string, Immediate> &data) {
    _assembly.output() << ".section .data" << "\n";
    _assembly.output() << "test: .double 2.0" << "\n";

    for (const auto &[name, value]: data) {
        _assembly.output() << name << ": .float " << value << "\n";
    }
}

void GASGenerator::_comment_instruction(Instruction &instruction) {
    auto printer = ILPrinter::print(instruction);
    _assembly.comment(printer.output().str());
}

void GASGenerator::_convert_int_to_int(const IntegerType &from, const Operand &expression,
                                       const IntegerType &to, const Operand &destination) {
    auto from_temporary = _temp1_register(from);
    _assembly.mov(from_temporary, expression);

    auto to_temporary = _temp1_register(to);
    if (to.size() > from.size()) _assembly.movsx(to_temporary, from_temporary);
    else _assembly.mov(to_temporary, from_temporary);

    _assembly.mov(destination, to_temporary);
}

void GASGenerator::_convert_float_to_float(const FloatingType &from, const Operand &expression,
                                           const FloatingType &to, const Operand &destination) {
    auto from_temporary = _temp1_register(from);
    _assembly.movss(from_temporary, expression);

    auto to_temporary = _temp1_register(to);
    if (from.size() == 32 && to.size() == 64) {
        _assembly.cvtss2sd(to_temporary, from_temporary);
        _assembly.movsd(destination, to_temporary);
    } else if (from.size() == 64 && to.size() == 32) {
        _assembly.cvtsd2ss(to_temporary, from_temporary);
        _assembly.movss(destination, to_temporary);
    }
}

void GASGenerator::_convert_int_to_float(const IntegerType &from, const Operand &expression,
                                         const FloatingType &to, const Operand &destination) {
    auto from_temporary = _temp1_register(from);
    if (from.size() < 32) {
        auto temp_type = std::make_shared<IntegerType>(32, from.sign());
        auto bigger_from_temporary = _temp1_register(*temp_type);
        _assembly.movsx(bigger_from_temporary, from_temporary);
        from_temporary = bigger_from_temporary;
    } else {
        _assembly.mov(from_temporary, expression);
    }

    auto to_temporary = _temp1_register(to);
    if (to.size() == 32) {
        _assembly.cvtsi2ss(to_temporary, from_temporary);
        _assembly.movss(destination, to_temporary);
    } else if (to.size() == 64) {
        _assembly.cvtsi2sd(to_temporary, from_temporary);
        _assembly.movsd(destination, to_temporary);
    }
}

void GASGenerator::_convert_float_to_int(const FloatingType &from, const Operand &expression,
                                         const IntegerType &to, const Operand &destination) {
    auto to_temporary = _temp1_register(to);

    auto bigger_to_temporary = to_temporary;
    if (from.size() < 32) {
        auto temp_type = std::make_shared<IntegerType>(32, to.sign());
        bigger_to_temporary = _temp1_register(*temp_type);
    }

    auto from_temporary = _temp1_register(from);
    if (from.size() == 32) {
        _assembly.movss(from_temporary, expression);
        _assembly.cvttss2si(bigger_to_temporary, from_temporary);
    } else {
        _assembly.movsd(from_temporary, expression);
        _assembly.cvttsd2si(bigger_to_temporary, from_temporary);
    }

    _assembly.mov(destination, to_temporary);
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
    if (auto integer = std::get_if<IntegerType>(&type)) {
        if (integer->sign()) return _assembly.imul(destination, src);
        else return _assembly.mul(destination, src);
    }

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
