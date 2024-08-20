#include "gas_generator.h"

#include <cassert>
#include <sstream>

#include "utils.h"
#include "instruction.h"

GASGenerator::GASGenerator() : _output() {
    _preamble();
}

void GASGenerator::visit(const LabelInstruction &node) {
    _label(node.symbol());

    if (node.symbol()->type() == Symbol::Type::Function) {
        _push("rbp");
        _mov("rbp", "rsp");
    }
}

void GASGenerator::visit(const ReturnInstruction &node) {
    _load(node.value(), "rax");
    _mov("rsp", "rbp");
    _pop("rbp");
    _ret();
}

void GASGenerator::visit(const BinaryInstruction &node) {
    _load(node.left(), "rax");
    _load(node.right(), "rbx");

    switch (node.type()) {
        case BinaryInstruction::Type::Add:
            _add("rax", "rbx");
            break;
        case BinaryInstruction::Type::Sub:
            _sub("rax", "rbx");
            break;
        case BinaryInstruction::Type::Mul:
            _imul("rax", "rbx");
            break;
        case BinaryInstruction::Type::Div:
            _idiv("rbx");
            break;
    }

    _store(node.result(), "rax");
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
            [&](const std::shared_ptr<Symbol> &symbol) {
                assert(symbol->type() == Symbol::Type::Temporary);
                _pop(destination);
            },
            [&](const long long &value) {
                auto operand = std::to_string(value);
                _mov(destination, operand);
            },
    }, operand);
}

void GASGenerator::_store(const Operand &operand, const std::string &src) {
    assert(std::holds_alternative<std::shared_ptr<Symbol>>(operand));

    auto symbol = std::get<std::shared_ptr<Symbol>>(operand);
    assert(symbol->type() == Symbol::Type::Temporary);

    _push(src);
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
