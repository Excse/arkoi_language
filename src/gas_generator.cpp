#include "gas_generator.h"

#include <sstream>

#include "instruction.h"
#include "il_printer.h"

std::ostream &operator<<(std::ostream &os, const Source &source) {
    std::visit([&os](const auto &arg) { os << arg; }, source);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Destination &destination) {
    std::visit([&os](const auto &arg) { os << arg; }, destination);
    return os;
}

GASGenerator::GASGenerator(bool debug) : _debug(debug) {
    _preamble();
}

void GASGenerator::visit(LabelInstruction &instruction) {
    _newline();
    _label(instruction.symbol());
}

void GASGenerator::visit(BeginInstruction &instruction) {
    _comment_instruction(instruction);
    _push(Register::RBP);
    _mov(Register::RBP, Register::RSP);
    if (instruction.local_size() != 0) {
        _sub(Register::RSP, Immediate(instruction.local_size()));
    }
    _newline();
}

void GASGenerator::visit(ReturnInstruction &instruction) {
    _comment_instruction(instruction);

    auto destination = Register(Register::Base::A, Register::type_to_register_size(instruction.type()));
    _mov(destination, _to_source(instruction.value()));

    _newline();
}

void GASGenerator::visit(BinaryInstruction &instruction) {
    _comment_instruction(instruction);

    auto left_reg = Register(Register::Base::A, Register::type_to_register_size(instruction.type()));
    _mov(left_reg, _to_source(instruction.left()));

    auto right_reg = Register(Register::Base::R11, Register::type_to_register_size(instruction.type()));
    _mov(right_reg, _to_source(instruction.right()));

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

    _mov(_to_destination(instruction.result()), left_reg);
    _newline();
}

void GASGenerator::visit(CastInstruction &instruction) {
    _comment_instruction(instruction);

    auto from_temporary = Register(Register::Base::A, Register::type_to_register_size(instruction.from()));
    _mov(from_temporary, _to_source(instruction.expression()));

    auto to_temporary = Register(Register::Base::A, Register::type_to_register_size(instruction.to()));

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

    _mov(_to_destination(instruction.result()), to_temporary);
    _newline();
}

void GASGenerator::visit(EndInstruction &instruction) {
    _comment_instruction(instruction);
    _mov(Register::RSP, Register::RBP);
    _pop(Register::RBP);
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

void GASGenerator::_movsx(const Destination &destination, const Source &src) {
    _output << "\tmovsx " << destination << ", " << src << "\n";
}

void GASGenerator::_mov(const Destination &destination, const Source &src) {
    _output << "\tmov " << destination << ", " << src << "\n";
}

void GASGenerator::_label(const std::shared_ptr<Symbol> &symbol) {
    _output << *symbol << ":\n";
}

void GASGenerator::_pop(const Destination &destination) {
    _output << "\tpop " << destination << "\n";
}

void GASGenerator::_push(const Source &src) {
    _output << "\tpush " << src << "\n";
}

void GASGenerator::_ret() {
    _output << "\tret\n";
}

void GASGenerator::_add(const Destination &destination, const Source &src) {
    _output << "\tadd " << destination << ", " << src << "\n";
}

void GASGenerator::_sub(const Destination &destination, const Source &src) {
    _output << "\tsub " << destination << ", " << src << "\n";
}

void GASGenerator::_idiv(const Source &dividend) {
    _output << "\tidiv " << dividend << "\n";
}

void GASGenerator::_imul(const Destination &destination, const Source &src) {
    _output << "\timul " << destination << ", " << src << "\n";
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

Destination GASGenerator::_to_destination(const Operand &operand) {
    if (auto memory = std::get_if<Memory>(&operand.data())) return *memory;
    if (auto reg = std::get_if<Register>(&operand.data())) return *reg;
    throw std::invalid_argument("This operand can't be converted to a destination operand");
}

Source GASGenerator::_to_source(const Operand &operand) {
    if (auto memory = std::get_if<Memory>(&operand.data())) return *memory;
    if (auto reg = std::get_if<Register>(&operand.data())) return *reg;
    if (auto immediate = std::get_if<Immediate>(&operand.data())) return *immediate;
    throw std::invalid_argument("This operand can't be converted to a source operand");
}
