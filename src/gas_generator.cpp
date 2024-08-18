#include "gas_generator.h"

#include "utils.h"
#include "tac.h"

GASGenerator::GASGenerator() : _output() {
    _preamble();
}

void GASGenerator::visit(const LabelInstruction &node) {
    _output += node.name() + ":\n";
}

void GASGenerator::visit(const ReturnInstruction &node) {
    std::string operand;
    std::visit(match {
            [&](const long long& value) { operand = std::to_string(value); },
            [](const auto&) { exit(1); }
    }, node.value());

    _output += "    mov rax, " + operand + "\n";
    _output += "    ret\n";
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
