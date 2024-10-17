#pragma once

#include "optimization.hpp"
#include "intermediate/instruction.hpp"
#include "utils/visitor.hpp"
#include "intermediate/cfg.hpp"

namespace arkoi {

class MemoryResolver : public SinglePass, intermediate::InstructionVisitor {
public:
    void new_cfg(CFG &) override { }

    void new_block(BasicBlock &block) override;

    void visit(intermediate::LabelInstruction &) override {};

    void visit(intermediate::BeginInstruction &instruction) override;

    void visit(intermediate::ReturnInstruction &instruction) override;

    void visit(intermediate::BinaryInstruction &instruction) override;

    void visit(intermediate::CastInstruction &instruction) override;

    void visit(intermediate::CallInstruction &instruction) override;

    void visit(intermediate::ArgumentInstruction &instruction) override;

    void visit(intermediate::IfNotInstruction &instruction) override;

    void visit(intermediate::StoreInstruction &instruction) override;

    void visit(intermediate::GotoInstruction &) override {};

    void visit(intermediate::EndInstruction &) override;

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
    intermediate::BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
    size_t _constant_index{};
};

}