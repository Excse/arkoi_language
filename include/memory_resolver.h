#ifndef ARKOI_LANGUAGE_MEMORY_RESOLVER_H
#define ARKOI_LANGUAGE_MEMORY_RESOLVER_H

#include "instruction.h"
#include "visitor.h"

class MemoryResolver : public InstructionVisitor {
public:
    MemoryResolver() : _offsets(), _current_begin(nullptr) {}

    void visit(LabelInstruction &node) override;

    void visit(BeginInstruction &node) override;

    void visit(ReturnInstruction &node) override;

    void visit(BinaryInstruction &node) override;

    void visit(EndInstruction &node) override;

private:
    void _resolve_operand(Operand &operand);

private:
    std::unordered_map<std::shared_ptr<Symbol>, size_t> _offsets;
    BeginInstruction *_current_begin;
};

#endif //ARKOI_LANGUAGE_MEMORY_RESOLVER_H
