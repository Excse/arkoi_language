#pragma once

#include "optimization.hpp"
#include "intermediate/instruction.hpp"
#include "utils/visitor.hpp"
#include "intermediate/cfg.hpp"

namespace arkoi {

class MemoryResolver : public SinglePass, intermediate::Visitor {
public:
    void new_cfg(CFG &) override { }

    void new_block(BasicBlock &block) override;

    void visit(intermediate::Label &) override {};

    void visit(intermediate::Begin &instruction) override;

    void visit(intermediate::Return &instruction) override;

    void visit(intermediate::Binary &instruction) override;

    void visit(intermediate::Cast &instruction) override;

    void visit(intermediate::Call &instruction) override;

    void visit(intermediate::Argument &instruction) override;

    void visit(intermediate::IfNot &instruction) override;

    void visit(intermediate::Store &instruction) override;

    void visit(intermediate::Goto &) override {};

    void visit(intermediate::End &) override;

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
    intermediate::Begin *_current_begin{};
    int64_t _parameter_offset{};
    size_t _constant_index{};
};

}