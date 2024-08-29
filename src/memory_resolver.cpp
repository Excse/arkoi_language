#include "memory_resolver.h"

#include <cassert>

#include "instruction.h"

void MemoryResolver::visit(LabelInstruction &) {}

void MemoryResolver::visit(BeginInstruction &node) {
    _current_begin = &node;
    _offsets.clear();
}

void MemoryResolver::visit(ReturnInstruction &node) {
    _resolve_operand(node.value());
}

void MemoryResolver::visit(BinaryInstruction &node) {
    _resolve_operand(node.result());
    _resolve_operand(node.left());
    _resolve_operand(node.right());
}

void MemoryResolver::visit(EndInstruction &) {}

void MemoryResolver::_resolve_operand(Operand &operand) {
    if (!std::holds_alternative<std::shared_ptr<Symbol>>(operand)) {
        return;
    }

    auto symbol = std::get<std::shared_ptr<Symbol>>(operand);
    assert(symbol->type() == Symbol::Type::Temporary);

    auto found = _offsets.find(symbol);
    size_t offset;

    if (found != _offsets.end()) {
        offset = found->second;
    } else {
        _current_begin->size() += 8;
        offset = _current_begin->size();

        _offsets[symbol] = offset;
    }

    operand = FPRelative(offset, false);
}
