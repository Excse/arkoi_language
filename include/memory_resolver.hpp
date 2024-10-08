#pragma once

#include "instruction.hpp"
#include "visitor.hpp"

class MemoryResolver : InstructionVisitor {
public:
    using Resolved = std::unordered_map<std::shared_ptr<Symbol>, Operand>;
    using ConstantData = std::unordered_map<std::string, Immediate>;

private:
    MemoryResolver() = default;

public:
    [[nodiscard]] static MemoryResolver resolve(const std::vector<std::unique_ptr<Instruction>> &instructions);

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

    [[nodiscard]] auto &resolved() const { return _resolved; }

    [[nodiscard]] auto &data() const { return _data; }

private:
    [[nodiscard]] Operand _resolve_operand(const Operand &operand);

    [[nodiscard]] Operand _resolve_immediate(const Immediate &immediate);

    [[nodiscard]] Operand _resolve_symbol(const std::shared_ptr<Symbol> &symbol);

    [[nodiscard]] Operand _resolve_temporary(const TemporarySymbol &symbol);

    [[nodiscard]] Operand _resolve_parameter(const ParameterSymbol &symbol);

    [[nodiscard]] static std::optional<Register> _resolve_argument(const ParameterSymbol &symbol);

    [[nodiscard]] static int64_t _type_to_byte_size(const Type &type);

    [[nodiscard]] static Size _byte_size_to_size(int64_t bytes);

private:
    BeginInstruction *_current_begin{};
    int64_t _parameter_offset{};
    int64_t _data_index{};
    Resolved _resolved{};
    ConstantData _data{};
};