#ifndef ARKOI_LANGUAGE_MEMORY_RESOLVER_H
#define ARKOI_LANGUAGE_MEMORY_RESOLVER_H

#include "instruction.h"
#include "visitor.h"

class MemoryResolver : public InstructionVisitor {
public:
    MemoryResolver() : _offsets(), _current_begin(nullptr), _parameter_offset(0) {}

    void visit(LabelInstruction &node) override;

    void visit(BeginInstruction &node) override;

    void visit(ReturnInstruction &node) override;

    void visit(BinaryInstruction &node) override;

    void visit(EndInstruction &node) override;

private:
    Operand _resolve_operand(const Operand &operand);

private:
    std::unordered_map<std::shared_ptr<Symbol>, size_t> _offsets;
    BeginInstruction *_current_begin;
    size_t _parameter_offset;
};

#endif //ARKOI_LANGUAGE_MEMORY_RESOLVER_H
