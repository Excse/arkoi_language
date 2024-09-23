#include "gas_generator.h"

#include <iomanip>
#include <sstream>

#include "instruction.h"
#include "il_printer.h"

static const auto RBP = std::make_shared<Register>(Register::Base::BP, Register::Size::QWORD);
static const auto RSP = std::make_shared<Register>(Register::Base::SP, Register::Size::QWORD);

GASGenerator::GASGenerator(bool debug) : _debug(debug) {
    _preamble();
}

void GASGenerator::visit(LabelInstruction &instruction) {
    _newline();
    _label(instruction.symbol());
}

void GASGenerator::visit(BeginInstruction &instruction) {
    _comment_instruction(instruction);
    _push(RBP);
    _mov(RBP, RSP);
    if (instruction.local_size() != 0) {
        _sub(RSP, std::make_shared<Immediate>(instruction.local_size()));
    }
    _newline();
}

void GASGenerator::visit(ReturnInstruction &instruction) {
    _comment_instruction(instruction);

    auto is_integer = std::dynamic_pointer_cast<IntegerType>(instruction.type());
    auto is_floating = std::dynamic_pointer_cast<FloatingType>(instruction.type());
    auto is_double = is_floating && is_floating->size() == 64;
    auto is_float = is_floating && is_floating->size() == 32;

    auto destination = _destination_register(*instruction.type());
    if (is_integer) {
        _mov(destination, instruction.value());
    } else if (is_float) {
        _movss(destination, instruction.value());
    } else if (is_double) {
        _movsd(destination, instruction.value());
    }

    _newline();
}

void GASGenerator::visit(BinaryInstruction &instruction) {
    _comment_instruction(instruction);

    auto is_integer = std::dynamic_pointer_cast<IntegerType>(instruction.type());
    auto is_signed = is_integer && is_integer->sign();
    auto is_floating = std::dynamic_pointer_cast<FloatingType>(instruction.type());
    auto is_double = is_floating && is_floating->size() == 64;
    auto is_float = is_floating && is_floating->size() == 32;

    auto left_reg = _temp1_register(*instruction.type());
    auto right_reg = _temp2_register(*instruction.type());
    if (is_integer) {
        _mov(left_reg, instruction.left());
        _mov(right_reg, instruction.right());

        if (instruction.op() == BinaryInstruction::Operator::Add) _add(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Sub) _sub(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Div && is_signed) _idiv(right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Div && !is_signed) _div(right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Mul && is_signed) _imul(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Mul && !is_signed) _mul(left_reg, right_reg);

        _mov(instruction.result(), left_reg);
    } else if (is_float) {
        _movss(left_reg, instruction.left());
        _movss(right_reg, instruction.right());

        if (instruction.op() == BinaryInstruction::Operator::Add) _addss(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Sub) _subss(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Div) _divss(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Mul) _mulss(left_reg, right_reg);

        _movss(instruction.result(), left_reg);
    } else if (is_double) {
        _movsd(left_reg, instruction.left());
        _movsd(right_reg, instruction.right());

        if (instruction.op() == BinaryInstruction::Operator::Add) _addsd(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Sub) _subsd(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Div) _divsd(left_reg, right_reg);
        if (instruction.op() == BinaryInstruction::Operator::Mul) _mulsd(left_reg, right_reg);

        _movsd(instruction.result(), left_reg);
    }

    _newline();
}

void GASGenerator::visit(CastInstruction &instruction) {
    _comment_instruction(instruction);

    auto from_integer = std::dynamic_pointer_cast<IntegerType>(instruction.from());
    auto to_integer = std::dynamic_pointer_cast<IntegerType>(instruction.to());
    if (from_integer && to_integer) {
        auto from_temporary = _temp1_register(*instruction.from());
        auto to_temporary = _temp1_register(*instruction.to());
        _mov(from_temporary, instruction.expression());

        if (to_integer->size() > from_integer->size()) {
            _movsx(to_temporary, from_temporary);
        } else if (to_integer->size() < from_integer->size()) {
            _mov(to_temporary, from_temporary);
        }

        _mov(instruction.result(), to_temporary);
        _newline();
        return;
    }

    auto from_floating = std::dynamic_pointer_cast<FloatingType>(instruction.from());
    auto to_floating = std::dynamic_pointer_cast<FloatingType>(instruction.to());
    if (from_floating && to_floating) {
        auto from_temporary = _temp1_register(*instruction.from());
        auto to_temporary = _temp1_register(*instruction.to());
        _movss(from_temporary, instruction.expression());

        if (from_floating->size() == 32 && to_floating->size() == 64) {
            _cvtss2sd(to_temporary, from_temporary);
            _movsd(instruction.result(), to_temporary);
        } else if (from_floating->size() == 64 && to_floating->size() == 32) {
            _cvtsd2ss(to_temporary, from_temporary);
            _movss(instruction.result(), to_temporary);
        }

        _newline();
        return;
    }

    if (from_integer && to_floating) {
        auto to_temporary = _temp1_register(*instruction.to());
        auto from_temporary = _temp1_register(*instruction.from());

        if (from_integer->size() < 32) {
            auto temp_type = std::make_shared<IntegerType>(32, from_integer->sign());
            auto bigger_from_temporary = _temp1_register(*temp_type);
            _movsx(bigger_from_temporary, from_temporary);
            from_temporary = bigger_from_temporary;
        } else {
            _mov(from_temporary, instruction.expression());
        }

        if (to_floating->size() == 32) {
            _cvtsi2ss(to_temporary, from_temporary);
            _movss(instruction.result(), to_temporary);
        } else if (to_floating->size() == 64) {
            _cvtsi2sd(to_temporary, from_temporary);
            _movsd(instruction.result(), to_temporary);
        }

        _newline();
        return;
    }

    if (from_floating && to_integer) {
        auto from_temporary = _temp1_register(*instruction.from());
        auto to_temporary = _temp1_register(*instruction.to());

        auto bigger_to_temporary = to_temporary;
        if (to_integer->size() < 32) {
            auto temp_type = std::make_shared<IntegerType>(32, to_integer->sign());
            bigger_to_temporary = _temp1_register(*temp_type);
        }

        if (from_floating->size() == 32) {
            _movss(from_temporary, instruction.expression());
            _cvttss2si(bigger_to_temporary, from_temporary);
        } else {
            _movsd(from_temporary, instruction.expression());
            _cvttsd2si(bigger_to_temporary, from_temporary);
        }

        _mov(instruction.result(), to_temporary);
        _newline();
        return;
    }

    throw std::runtime_error("Cast Instruction not implemented yet.");
}

void GASGenerator::visit(EndInstruction &instruction) {
    _comment_instruction(instruction);
    _mov(RSP, RBP);
    _pop(RBP);
    _ret();
    _newline();
}

void GASGenerator::_preamble() {
    _output << R"(
.intel_syntax noprefix
.section .text
.global _start

_start:
    movsd xmm0, 2.0
    call main

    mov rdi, rax
    mov rax, 60
    syscall
)";
}

void GASGenerator::_cvttsd2si(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvttsd2si " << *destination << ", " << *src << "\n";
}

void GASGenerator::_cvttss2si(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvttss2si " << *destination << ", " << *src << "\n";
}

void GASGenerator::_cvtss2sd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvtss2sd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_cvtsd2ss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvtsd2ss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_cvtsi2ss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvtsi2ss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_cvtsi2sd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tcvtsi2sd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_movsx(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmovsx " << *destination << ", " << *src << "\n";
}

void GASGenerator::_movss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmovss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_movsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmovsd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_mov(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmov " << *destination << ", " << *src << "\n";
}

void GASGenerator::_label(const std::shared_ptr<Symbol> &symbol) {
    _output << *symbol << ":\n";
}

void GASGenerator::_pop(const std::shared_ptr<Operand> &destination) {
    _output << "\tpop " << *destination << "\n";
}

void GASGenerator::_push(const std::shared_ptr<Operand> &src) {
    _output << "\tpush " << *src << "\n";
}

void GASGenerator::_ret() {
    _output << "\tret\n";
}

void GASGenerator::_add(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tadd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_addsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\taddsd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_addss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\taddss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_sub(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tsub " << *destination << ", " << *src << "\n";
}

void GASGenerator::_subsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tsubsd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_subss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tsubss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_idiv(const std::shared_ptr<Operand> &dividend) {
    _output << "\tidiv " << *dividend << "\n";
}

void GASGenerator::_div(const std::shared_ptr<Operand> &dividend) {
    _output << "\tdiv " << *dividend << "\n";
}

void GASGenerator::_divsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tdivsd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_divss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tdivss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_imul(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\timul " << *destination << ", " << *src << "\n";
}

void GASGenerator::_mul(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmul " << *destination << ", " << *src << "\n";
}

void GASGenerator::_mulsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmulsd " << *destination << ", " << *src << "\n";
}

void GASGenerator::_mulss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmulss " << *destination << ", " << *src << "\n";
}

void GASGenerator::_comment_instruction(Instruction &instruction) {
    if (!_debug) return;

    instruction.accept(_printer);
    _output << "\t# " << _printer.output().str();
    _printer.clear();
}

void GASGenerator::_newline() {
    if (!_debug) return;
    _output << "\n";
}

std::shared_ptr<Register> GASGenerator::_destination_register(const Type &type) {
    if (dynamic_cast<const IntegerType *>(&type)) {
        return std::make_shared<Register>(Register::Base::A, Register::type_to_register_size(type));
    } else if (dynamic_cast<const FloatingType *>(&type)) {
        return std::make_shared<Register>(Register::Base::XMM0, Register::type_to_register_size(type));
    }

    throw std::invalid_argument("This type is not implemented.");
}

std::shared_ptr<Register> GASGenerator::_temp1_register(const Type &type) {
    if (dynamic_cast<const IntegerType *>(&type)) {
        return std::make_shared<Register>(Register::Base::A, Register::type_to_register_size(type));
    } else if (dynamic_cast<const FloatingType *>(&type)) {
        return std::make_shared<Register>(Register::Base::XMM11, Register::type_to_register_size(type));
    }

    throw std::invalid_argument("This type is not implemented.");
}

std::shared_ptr<Register> GASGenerator::_temp2_register(const Type &type) {
    if (dynamic_cast<const IntegerType *>(&type)) {
        return std::make_shared<Register>(Register::Base::R11, Register::type_to_register_size(type));
    } else if (dynamic_cast<const FloatingType *>(&type)) {
        return std::make_shared<Register>(Register::Base::XMM12, Register::type_to_register_size(type));
    }

    throw std::invalid_argument("This type is not implemented.");
}

std::string GASGenerator::double_to_hex(double value) {
    uint64_t hex_value = *reinterpret_cast<uint64_t *>(&value);
    std::stringstream ss;
    ss << std::hex << std::showbase << std::setw(16) << std::setfill('0') << hex_value;
    return ss.str();
}

std::string GASGenerator::float_to_hex(float value) {
    uint32_t hex_value = *reinterpret_cast<uint32_t *>(&value);
    std::stringstream ss;
    ss << std::hex << std::showbase << std::setw(8) << std::setfill('0') << hex_value;
    return ss.str();
}
