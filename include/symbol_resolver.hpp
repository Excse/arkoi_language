#pragma once

#include "optimization.hpp"
#include "instruction.hpp"
#include "visitor.hpp"
#include "cfg.hpp"

class SymbolResolver : public OptimizationPass, InstructionVisitor {
public:
    void new_cfg(CFG &) override {}

    void new_block(BasicBlock &) override {}

    void instruction(Instruction &instruction) override;

    void visit(LabelInstruction &) override {};

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(CallInstruction &instruction) override;

    void visit(ArgumentInstruction &instruction) override;

    void visit(IfNotInstruction &instruction) override;

    void visit(GotoInstruction &) override {};

    void visit(EndInstruction &) override;

private:
    [[nodiscard]] Operand _resolve_operand(const Operand &operand);

    [[nodiscard]] Operand _resolve_symbol(const std::shared_ptr<Symbol> &symbol);

    [[nodiscard]] Operand _resolve_temporary(const TemporarySymbol &symbol);

    [[nodiscard]] Operand _resolve_parameter(const ParameterSymbol &symbol);

    [[nodiscard]] static std::optional<Register> _resolve_parameter_register(const ParameterSymbol &symbol);

private:
    std::unordered_map<std::shared_ptr<Symbol>, Operand> _resolved{};
    BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
};