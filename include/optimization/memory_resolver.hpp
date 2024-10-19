#pragma once

#include "optimization.hpp"
#include "il/instruction.hpp"
#include "utils/visitor.hpp"
#include "il/cfg.hpp"

class MemoryResolver : public SinglePass, il::Visitor {
public:
    void new_cfg(CFG &) override { }

    void new_block(BasicBlock &block) override;

    void visit(il::Label &) override {};

    void visit(il::Begin &instruction) override;

    void visit(il::Return &instruction) override;

    void visit(il::Binary &instruction) override;

    void visit(il::Cast &instruction) override;

    void visit(il::Call &instruction) override;

    void visit(il::Argument &instruction) override;

    void visit(il::IfNot &instruction) override;

    void visit(il::Store &instruction) override;

    void visit(il::Goto &) override {};

    void visit(il::End &) override;

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
    il::Begin *_current_begin{};
    int64_t _parameter_offset{};
    size_t _constant_index{};
};
