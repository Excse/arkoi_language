#pragma once

#include "backend/x86_64/operand.hpp"
#include "il/instruction.hpp"
#include "il/cfg.hpp"

namespace x86_64 {

struct ConstantData {
    x86_64::Operand operand;
    std::string name;
};

class OperandResolver : il::Visitor {
public:
    OperandResolver() : _parameter_offset(8) {}

    [[nodiscard]] static OperandResolver resolve(CFG &cfg);

    [[nodiscard]] x86_64::Operand resolve_operand(const il::Operand &operand);

    [[nodiscard]] int64_t local_size() const { return _local_size; }

    [[nodiscard]] auto &constants() const { return _constants; }

private:
    void visit(il::Begin &instruction) override;

    void visit(il::Label &) override {};

    void visit(il::Return &instruction) override;

    void visit(il::Binary &instruction) override;

    void visit(il::Cast &instruction) override;

    void visit(il::Call &instruction) override;

    void visit(il::Argument &instruction) override;

    void visit(il::IfNot &instruction) override;

    void visit(il::Store &instruction) override;

    void visit(il::Goto &) override {};

    void visit(il::End &instruction) override;

    [[nodiscard]] x86_64::Operand _resolve_symbol(const Symbol &symbol);

    [[nodiscard]] x86_64::Operand _resolve_constant(const il::Constant &constant);

    [[nodiscard]] x86_64::Operand _resolve_temporary(const TemporarySymbol &symbol);

    [[nodiscard]] x86_64::Operand _resolve_parameter(const Symbol &symbol,
                                                     size_t &int_index,
                                                     size_t &sse_index);

    [[nodiscard]] std::optional<Register> _resolve_parameter_register(const ParameterSymbol &symbol,
                                                                      size_t &int_index,
                                                                      size_t &sse_index);

private:
    std::unordered_map<il::Constant, ConstantData> _constants{};
    std::unordered_map<Symbol, x86_64::Operand> _resolved{};
    int64_t _parameter_offset;
    size_t _constant_index{};
    int64_t _local_size{};
};

}