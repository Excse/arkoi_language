#pragma once

#include "optimization.hpp"
#include "instruction.hpp"
#include "visitor.hpp"
#include "cfg.hpp"

class MemoryResolver : public SinglePass, InstructionVisitor {
public:
    void new_cfg(CFG &) override { }

    void new_block(BasicBlock &block) override;

    void visit(LabelInstruction &) override {};

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(CallInstruction &instruction) override;

    void visit(ArgumentInstruction &instruction) override;

    void visit(IfNotInstruction &instruction) override;

    void visit(StoreInstruction &instruction) override;

    void visit(GotoInstruction &) override {};

    void visit(EndInstruction &) override;

    [[nodiscard]] auto &constants() const { return _constants; }

private:
    [[nodiscard]] Operand _resolve_operand(const Operand &operand);

    [[nodiscard]] Operand _resolve_symbol(const std::shared_ptr<Symbol> &symbol);

    [[nodiscard]] Operand _resolve_immediate(const Immediate &immediate);

    [[nodiscard]] Operand _resolve_temporary(const TemporarySymbol &symbol);

    [[nodiscard]] Operand _resolve_parameter(const ParameterSymbol &symbol);

    [[nodiscard]] static std::optional<Register> _resolve_parameter_register(const ParameterSymbol &symbol);

private:
    std::unordered_map<std::shared_ptr<Symbol>, Operand> _resolved{};
    std::unordered_map<std::string, Immediate> _constants{};
    BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
    size_t _constant_index{};
};