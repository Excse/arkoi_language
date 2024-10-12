#include "il_printer.hpp"

#include <iostream>

#include "symbol_table.hpp"
#include "instruction.hpp"
#include "utils.hpp"

ILPrinter ILPrinter::print(std::vector<CFG> &cfgs) {
    ILPrinter printer;

    auto visit_instructions = [&](BasicBlock &block) {
        for (auto &instruction: block.instructions()) {
            instruction->accept(printer);
        }
    };

    for (auto &cfg: cfgs) {
        cfg.depth_first_search(visit_instructions);
    }

    return printer;
}

ILPrinter ILPrinter::print(Instruction &instruction) {
    ILPrinter printer;

    instruction.accept(printer);

    return printer;
}

void ILPrinter::visit(LabelInstruction &instruction) {
    _output << "LABEL " << *instruction.symbol() << ":\n";
}

void ILPrinter::visit(BeginInstruction &instruction) {
    _output << "BEGIN " << *instruction.label();
    if (instruction.local_size()) _output << " LOCALS " << " " << instruction.local_size();
    _output << "\n";
}

void ILPrinter::visit(ReturnInstruction &instruction) {
    _output << "RETURN " << instruction.value() << "\n";
}

void ILPrinter::visit(BinaryInstruction &instruction) {
    _output << instruction.result() << " = " << to_string(instruction.op()) << " " << instruction.left() << ", "
            << instruction.right() << "\n";
}

void ILPrinter::visit(CastInstruction &instruction) {
    _output << instruction.result() << " = CAST " << instruction.expression() << " FROM @" << instruction.from()
            << " TO @" << instruction.to() << "\n";
}

void ILPrinter::visit(EndInstruction &) {
    _output << "END\n";
}

void ILPrinter::visit(CallInstruction &instruction) {
    _output << instruction.result() << " = CALL " << *instruction.symbol() << "\n";
}

void ILPrinter::visit(ArgumentInstruction &instruction) {
    _output << "ARG " << instruction.expression() << "\n";
}

void ILPrinter::visit(GotoInstruction &instruction) {
    _output << "GOTO " << *instruction.label() << "\n";
}

void ILPrinter::visit(IfNotInstruction &instruction) {
    _output << "IF NOT " << instruction.condition() << " GOTO " << *instruction.label() << "\n";
}

void ILPrinter::visit(StoreInstruction &instruction) {
    _output << instruction.result() << " = " << instruction.value() << "\n";
}
