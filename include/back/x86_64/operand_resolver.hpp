#pragma once

#include "back/x86_64/operand.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"

namespace arkoi::back::x86_64 {

struct ConstantData {
    x86_64::Operand operand;
    std::string name;
};

class OperandResolver : mid::Visitor {
public:
    OperandResolver() : _parameter_offset(8) {}

    [[nodiscard]] static OperandResolver resolve(mid::Function &function);

    [[nodiscard]] x86_64::Operand resolve_operand(const mid::Operand &operand);

    [[nodiscard]] int64_t local_size() const { return _local_size; }

    [[nodiscard]] auto &constants() const { return _constants; }

private:
    void visit(mid::Begin &instruction) override;

    void visit(mid::Label &) override {};

    void visit(mid::Return &instruction) override;

    void visit(mid::Binary &instruction) override;

    void visit(mid::Cast &instruction) override;

    void visit(mid::Call &instruction) override;

    void visit(mid::If &instruction) override;

    void visit(mid::Store &instruction) override;

    void visit(mid::Goto &) override {};

    void visit(mid::End &instruction) override;

    [[nodiscard]] x86_64::Operand _resolve_variable(const mid::Variable &variable);

    [[nodiscard]] x86_64::Operand _resolve_constant(const mid::Constant &constant);

    [[nodiscard]] x86_64::Operand _resolve_temporary(const symbol::Temporary &symbol);

    [[nodiscard]] x86_64::Operand _resolve_parameter(const mid::Variable &variable,
                                                     size_t &int_index,
                                                     size_t &sse_index);

    [[nodiscard]] static std::optional<Register> _resolve_parameter_register(const symbol::Parameter &symbol,
                                                                             size_t &int_index,
                                                                             size_t &sse_index);

private:
    std::unordered_map<mid::Variable, x86_64::Operand> _resolved{};
    std::unordered_map<mid::Constant, ConstantData> _constants{};
    int64_t _parameter_offset;
    int64_t _local_size{};
};

} // namespace arkoi::back::x86_64