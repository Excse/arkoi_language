#include "gas_generator.h"

#include <cassert>
#include <sstream>

#include "instruction.h"
#include "il_printer.h"
#include "utils.h"

GASGenerator::GASGenerator(bool debug) : _output(), _printer(), _debug(debug) {
    _preamble();
}

void GASGenerator::visit(LabelInstruction &node) {
    _comment_instruction(node);
    _label(node.symbol());
}

void GASGenerator::visit(BeginInstruction &node) {
    _comment_instruction(node);
    _push("rbp");
    _mov("rbp", "rsp");
    _sub("rsp", to_string(node.size()));
    _newline();
}

void GASGenerator::visit(ReturnInstruction &node) {
    _comment_instruction(node);
    _load(node.value(), "rax");
    _newline();
}

void GASGenerator::visit(BinaryInstruction &node) {
    _comment_instruction(node);
    _load(node.left(), "rax");
    _load(node.right(), "r11");

    switch (node.type()) {
        case BinaryInstruction::Type::Add: {
            _add("rax", "r11");
            break;
        }
        case BinaryInstruction::Type::Sub: {
            _sub("rax", "r11");
            break;
        }
        case BinaryInstruction::Type::Mul: {
            _imul("rax", "r11");
            break;
        }
        case BinaryInstruction::Type::Div: {
            _idiv("r11");
            break;
        }
    }

    _store(node.result(), "rax");
    _newline();
}

void GASGenerator::visit(EndInstruction &node) {
    _comment_instruction(node);
    _mov("rsp", "rbp");
    _pop("rbp");
    _ret();
    _newline();
}

void GASGenerator::_preamble() {
    _output << R"(
.intel_syntax noprefix
.section .text
.global _start

_start:
    call main

    mov rdi, rax
    mov rax, 60
    syscall
)";
}

void GASGenerator::_load(const Operand &operand, const std::string &destination) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &) {
                throw std::invalid_argument("Cannot load symbols");
            },
            [&](const FPRelative &relative) {
                _mov(destination, to_string(relative));
            },
            [&](const Register &reg) {
                _mov(destination, to_string(reg));
            },
            [&](const long long &value) {
                _mov(destination, to_string(value));
            },
    }, operand);
}

void GASGenerator::_store(const Operand &operand, const std::string &src) {
    assert(std::holds_alternative<FPRelative>(operand));
    _mov(to_string(operand), src);
}

void GASGenerator::_mov(const std::string &destination, const std::string &src) {
    _output << "\tmov " << destination << ", " << src << "\n";
}

void GASGenerator::_label(const std::shared_ptr<Symbol> &symbol) {
    _output << "\n" << *symbol << ":\n";
}

void GASGenerator::_pop(const std::string &destination) {
    _output << "\tpop " << destination << "\n";
}

void GASGenerator::_push(const std::string &src) {
    _output << "\tpush " << src << "\n";
}

void GASGenerator::_ret() {
    _output << "\tret\n";
}

void GASGenerator::_add(const std::string &destination, const std::string &src) {
    _output << "\tadd " << destination << ", " << src << "\n";
}

void GASGenerator::_sub(const std::string &destination, const std::string &src) {
    _output << "\tsub " << destination << ", " << src << "\n";
}

void GASGenerator::_idiv(const std::string &dividend) {
    _output << "\tidiv " << dividend << "\n";
}

void GASGenerator::_imul(const std::string &destination, const std::string &src) {
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