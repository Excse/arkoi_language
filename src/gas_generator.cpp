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

    auto destination = std::make_shared<Register>(Register::Base::A,
                                                  Register::type_to_register_size(instruction.type()));
    _mov(destination, instruction.value());

    _newline();
}

void GASGenerator::visit(BinaryInstruction &instruction) {
    _comment_instruction(instruction);

    auto left_reg = std::make_shared<Register>(Register::Base::A, Register::type_to_register_size(instruction.type()));
    _mov(left_reg, instruction.left());

    auto right_reg = std::make_shared<Register>(Register::Base::R11,
                                                Register::type_to_register_size(instruction.type()));
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
            _idiv(right_reg);
            break;
        }
    }

    _mov(instruction.result(), left_reg);
    _newline();
}

void GASGenerator::visit(CastInstruction &instruction) {
    _comment_instruction(instruction);

    auto from_temporary = std::make_shared<Register>(Register::Base::A,
                                                     Register::type_to_register_size(instruction.from()));
    _mov(from_temporary, instruction.expression());

    auto to_temporary = std::make_shared<Register>(Register::Base::A,
                                                   Register::type_to_register_size(instruction.to()));

    auto from_integer = std::dynamic_pointer_cast<IntegerType>(instruction.from());
    auto to_integer = std::dynamic_pointer_cast<IntegerType>(instruction.to());
    if (from_integer && to_integer) {
        if (to_integer->size() > from_integer->size()) {
            _movsx(to_temporary, from_temporary);
        } else if (to_integer->size() < from_integer->size()) {
            // Do nothing, just use to_temporary as they are the same register just in different sizes
        }
    } else {
        throw std::runtime_error("Cast Instruction not implemented yet.");
    }

    _mov(instruction.result(), to_temporary);
    _newline();
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

void GASGenerator::_movsx(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src) {
    _output << "\tmovsx " << *destination << ", " << *src << "\n";
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