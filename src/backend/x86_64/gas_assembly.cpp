#include "backend/x86_64/gas_assembly.hpp"

#include <iomanip>

void Assembly::cvttsd2si(const Operand &destination, const Operand &src) {
    _output << "\tcvttsd2si " << destination << ", " << src << "\n";
}

void Assembly::cvttss2si(const Operand &destination, const Operand &src) {
    _output << "\tcvttss2si " << destination << ", " << src << "\n";
}

void Assembly::cvtss2sd(const Operand &destination, const Operand &src) {
    _output << "\tcvtss2sd " << destination << ", " << src << "\n";
}

void Assembly::cvtsd2ss(const Operand &destination, const Operand &src) {
    _output << "\tcvtsd2ss " << destination << ", " << src << "\n";
}

void Assembly::cvtsi2ss(const Operand &destination, const Operand &src) {
    _output << "\tcvtsi2ss " << destination << ", " << src << "\n";
}

void Assembly::cvtsi2sd(const Operand &destination, const Operand &src) {
    _output << "\tcvtsi2sd " << destination << ", " << src << "\n";
}

void Assembly::movsx(const Operand &destination, const Operand &src) {
    _output << "\tmovsx " << destination << ", " << src << "\n";
}

void Assembly::movzx(const Operand &destination, const Operand &src) {
    _output << "\tmovzx " << destination << ", " << src << "\n";
}

void Assembly::movsxd(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovsxd " << destination << ", " << src << "\n";
}

void Assembly::movss(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovss " << destination << ", " << src << "\n";
}

void Assembly::movsd(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmovsd " << destination << ", " << src << "\n";
}

void Assembly::mov(const Operand &destination, const Operand &src) {
    if (src == destination) return;
    _output << "\tmov " << destination << ", " << src << "\n";
}

void Assembly::label(const Symbol &symbol, bool newline) {
    _output << symbol << ": ";
    if (newline) _output << "\n";
}

void Assembly::jmp(const Operand &destination) {
    _output << "\tjmp " << destination << "\n";
}

void Assembly::je(const Operand &destination) {
    _output << "\tje " << destination << "\n";
}

void Assembly::pop(const Operand &destination) {
    _output << "\tpop " << destination << "\n";
}

void Assembly::push(const Operand &src) {
    _output << "\tpush " << src << "\n";
}

void Assembly::ret() {
    _output << "\tret\n";
}

void Assembly::ucomiss(const Operand &destination, const Operand &source) {
    _output << "\tucomiss " << destination << ", " << source << "\n";
}

void Assembly::ucomisd(const Operand &destination, const Operand &source) {
    _output << "\tucomisd " << destination << ", " << source << "\n";
}

void Assembly::pxor(const Operand &destination, const Operand &source) {
    _output << "\tpxor " << destination << ", " << source << "\n";
}

void Assembly::cmp(const Operand &first, const Operand &second) {
    _output << "\tcmp " << first << ", " << second << "\n";
}

void Assembly::setne(const Operand &destination) {
    _output << "\tsetne " << destination << "\n";
}

void Assembly::add(const Operand &destination, const Operand &src) {
    _output << "\tadd " << destination << ", " << src << "\n";
}

void Assembly::addsd(const Operand &destination, const Operand &src) {
    _output << "\taddsd " << destination << ", " << src << "\n";
}

void Assembly::addss(const Operand &destination, const Operand &src) {
    _output << "\taddss " << destination << ", " << src << "\n";
}

void Assembly::sub(const Operand &destination, const Operand &src) {
    _output << "\tsub " << destination << ", " << src << "\n";
}

void Assembly::subsd(const Operand &destination, const Operand &src) {
    _output << "\tsubsd " << destination << ", " << src << "\n";
}

void Assembly::subss(const Operand &destination, const Operand &src) {
    _output << "\tsubss " << destination << ", " << src << "\n";
}

void Assembly::idiv(const Operand &dividend) {
    _output << "\tidiv " << dividend << "\n";
}

void Assembly::div(const Operand &dividend) {
    _output << "\tdiv " << dividend << "\n";
}

void Assembly::divsd(const Operand &destination, const Operand &src) {
    _output << "\tdivsd " << destination << ", " << src << "\n";
}

void Assembly::divss(const Operand &destination, const Operand &src) {
    _output << "\tdivss " << destination << ", " << src << "\n";
}

void Assembly::imul(const Operand &destination, const Operand &src) {
    _output << "\timul " << destination << ", " << src << "\n";
}

void Assembly::mulsd(const Operand &destination, const Operand &src) {
    _output << "\tmulsd " << destination << ", " << src << "\n";
}

void Assembly::mulss(const Operand &destination, const Operand &src) {
    _output << "\tmulss " << destination << ", " << src << "\n";
}

void Assembly::directive(const std::string &name, const std::vector<std::string> &settings) {
    _output << name;

    for (size_t index = 0; index < settings.size(); index++) {
        const auto &setting = settings[index];
        _output << " " << setting;
        if (index != settings.size() - 1) _output << ",";
    }

    _output << "\n";
}

void Assembly::call(const Symbol &symbol) {
    _output << "\tcall " << symbol << "\n";
}

void Assembly::syscall() {
    _output << "\tsyscall\n";
}

void Assembly::comment(const std::string &comment) {
    _output << "\t# " << comment;
}

void Assembly::newline() {
    _output << "\n";
}
