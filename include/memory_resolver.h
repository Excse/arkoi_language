#ifndef ARKOI_LANGUAGE_MEMORY_RESOLVER_H
#define ARKOI_LANGUAGE_MEMORY_RESOLVER_H

#include "instruction.h"
#include "visitor.h"

class MemoryResolver : public InstructionVisitor {
public:
    MemoryResolver() : _resolved(), _current_begin(nullptr), _parameter_offset(0) {}

    void visit(LabelInstruction &) override {};

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(EndInstruction &) override {};

    [[nodiscard]] auto &resolved() const { return _resolved; }

private:
    Operand _resolve_operand(const Operand &operand);

private:
    std::unordered_map<std::shared_ptr<Symbol>, Operand> _resolved;
    BeginInstruction *_current_begin;
    int64_t _parameter_offset;
};

#endif //ARKOI_LANGUAGE_MEMORY_RESOLVER_H
