#include "gas_generator.h"

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
    auto destination = _destination_register(*instruction.type());
    _mov(destination, instruction.value());
    _newline();
}

void GASGenerator::visit(BinaryInstruction &instruction) {
    _comment_instruction(instruction);

    auto left_reg = _temp1_register(*instruction.type());
    _mov(left_reg, instruction.left());

    auto right_reg = _temp2_register(*instruction.type());
    _mov(right_reg, instruction.right());

    switch (instruction.op()) {
        case BinaryInstruction::Operator::Add: {
            _add(left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Sub: {
            _sub(left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Mul: {
            _imul(left_reg, right_reg);
            break;
        }
        case BinaryInstruction::Operator::Div: {
            // TODO: Use div for unsigned and idiv for signed
            _idiv(right_reg);
            break;
        }
    }

    _mov(instruction.result(), left_reg);
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

    if(from_floating && to_integer) {
        auto from_temporary = _temp1_register(*instruction.from());
        auto to_temporary = _temp1_register(*instruction.to());

        auto bigger_to_temporary = to_temporary;
        if(to_integer->size() < 32) {
            auto temp_type = std::make_shared<IntegerType>(32, to_integer->sign());
            bigger_to_temporary = _temp1_register(*temp_type);
        }

        if(from_floating->size() == 32) {
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
    _output << R"(.intel_syntax noprefix
.section .text
.global _start

_start:
    mov rdi, 2
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

void GASGenerator::_sub(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tsub " << *destination << ", " << *src << "\n";
}

void GASGenerator::_idiv(const std::shared_ptr<Operand> &dividend) {
    _output << "\tidiv " << *dividend << "\n";
}

void GASGenerator::_imul(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\timul " << *destination << ", " << *src << "\n";
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
