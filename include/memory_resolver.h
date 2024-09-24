#pragma once

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
    [[nodiscard]] Operand _resolve_operand(const Operand &operand);

    [[nodiscard]] Operand _resolve_symbol(const Symbol &symbol);

    [[nodiscard]] Operand _resolve_temporary(const TemporarySymbol &symbol);

    [[nodiscard]] Operand _resolve_parameter(const ParameterSymbol &symbol);

    [[nodiscard]] static int64_t _type_to_byte_size(const Type &type);

private:
    std::unordered_map<const Symbol *, Operand> _resolved{};
    BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
};