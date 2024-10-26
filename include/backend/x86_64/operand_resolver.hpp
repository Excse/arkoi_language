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

    [[nodiscard]] static OperandResolver resolve(Function &function);

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

    void visit(il::If &instruction) override;

    void visit(il::Store &instruction) override;

    void visit(il::Goto &) override {};

    void visit(il::End &instruction) override;

    [[nodiscard]] x86_64::Operand _resolve_variable(const il::Variable &variable);

    [[nodiscard]] x86_64::Operand _resolve_constant(const il::Constant &constant);

    [[nodiscard]] x86_64::Operand _resolve_temporary(const symbol::Temporary &symbol);

    [[nodiscard]] x86_64::Operand _resolve_parameter(const il::Variable &variable,
                                                     size_t &int_index,
                                                     size_t &sse_index);

    [[nodiscard]] static std::optional<Register> _resolve_parameter_register(const symbol::Parameter &symbol,
                                                                             size_t &int_index,
                                                                             size_t &sse_index);

private:
    std::unordered_map<il::Variable, x86_64::Operand> _resolved{};
    std::unordered_map<il::Constant, ConstantData> _constants{};
    int64_t _parameter_offset;
    int64_t _local_size{};
};

}