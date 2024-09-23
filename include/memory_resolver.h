#ifndef ARKOI_LANGUAGE_MEMORY_RESOLVER_H
#define ARKOI_LANGUAGE_MEMORY_RESOLVER_H

#include "instruction.h"
#include "visitor.h"

class MemoryResolver : public InstructionVisitor {
public:
    void visit(LabelInstruction &) override {};

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(EndInstruction &) override {};

    void visit(CastInstruction &instruction) override;

    [[nodiscard]] auto &resolved() const { return _resolved; }

private:
    std::shared_ptr<Operand> _resolve_operand(const std::shared_ptr<Operand> &operand);

    static int64_t _type_to_byte_size(const std::shared_ptr<Type> &type);

private:
    std::unordered_map<std::shared_ptr<Symbol>, std::shared_ptr<Operand>> _resolved{};
    BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
};

#endif //ARKOI_LANGUAGE_MEMORY_RESOLVER_H
