#include "gas_assembly.h"

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

void Assembly::movss(const Operand &destination, const Operand &src) {
    _output << "\tmovss " << destination << ", " << src << "\n";
}

void Assembly::movsd(const Operand &destination, const Operand &src) {
    _output << "\tmovsd " << destination << ", " << src << "\n";
}

void Assembly::mov(const Operand &destination, const Operand &src) {
    _output << "\tmov " << destination << ", " << src << "\n";
}

void Assembly::label(const std::shared_ptr<Symbol> &symbol) {
    _output << *symbol << ":\n";
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

void Assembly::mul(const Operand &destination, const Operand &src) {
    _output << "\tmul " << destination << ", " << src << "\n";
}

void Assembly::mulsd(const Operand &destination, const Operand &src) {
    _output << "\tmulsd " << destination << ", " << src << "\n";
}

void Assembly::mulss(const Operand &destination, const Operand &src) {
    _output << "\tmulss " << destination << ", " << src << "\n";
}

void Assembly::comment(const std::string &comment) {
    _output << "\t# " << comment;
}

void Assembly::newline() {
    _output << "\n";
}

std::string Assembly::double_to_hex(double value) {
    uint64_t hex_value = *reinterpret_cast<uint64_t *>(&value);
    std::stringstream ss;
    ss << std::hex << std::showbase << std::setw(16) << std::setfill('0') << hex_value;
    return ss.str();
}

std::string Assembly::float_to_hex(float value) {
    uint32_t hex_value = *reinterpret_cast<uint32_t *>(&value);
    std::stringstream ss;
    ss << std::hex << std::showbase << std::setw(8) << std::setfill('0') << hex_value;
    return ss.str();
}