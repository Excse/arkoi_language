#include "gas_generator.h"

#include <cassert>

#include "utils.h"
#include "instruction.h"

GASGenerator::GASGenerator() : _output() {
    _preamble();
}

void GASGenerator::visit(const LabelInstruction &node) {
    _output += node.name() + ":\n";
}

void GASGenerator::visit(const ReturnInstruction &node) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) {
                assert(symbol->type() == Symbol::Type::Temporary);
                _output += "    pop rax\n";
            },
            [&](const long long &value) {
                auto operand = std::to_string(value);
                _output += "    mov rax, " + operand + "\n";
            },
    }, node.value());

    _output += "    ret\n";
}

void GASGenerator::visit(const BinaryInstruction &node) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) {
                assert(symbol->type() == Symbol::Type::Temporary);
                _output += "    pop rax\n";
            },
            [&](const long long &value) {
                _output += "    mov rax, " + std::to_string(value) + "\n";
            }
    }, node.left());

    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) {
                assert(symbol->type() == Symbol::Type::Temporary);
                _output += "    pop rbx\n";
            },
            [&](const long long &value) {
                _output += "    mov rbx, " + std::to_string(value) + "\n";
            }
    }, node.right());

    switch(node.type()) {
        case BinaryInstruction::Type::Add:
            _output += "    add rax, rbx\n";
            break;
        case BinaryInstruction::Type::Sub:
            _output += "    sub rax, rbx\n";
            break;
        case BinaryInstruction::Type::Mul:
            _output += "    imul rax, rbx\n";
            break;
        case BinaryInstruction::Type::Div:
            _output += "    idiv rbx\n";
            break;
    }

    _output += "    push rax\n";
}

void GASGenerator::_preamble() {
    _output += R"(
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
