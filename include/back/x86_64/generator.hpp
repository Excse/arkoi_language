#pragma once

#include "back/x86_64/operand_resolver.hpp"
#include "back/x86_64/assembly.hpp"
#include "back/x86_64/operand.hpp"
#include "mid/symbol_table.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"
#include "utils/visitor.hpp"

namespace arkoi::back::x86_64 {

class Generator : mid::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Generator generate(std::vector<mid::Function> &functions);

    void visit(mid::Label &instruction) override;

    void visit(mid::Begin &instruction) override;

    void visit(mid::Return &instruction) override;

    void visit(mid::Binary &memory) override;

    void visit(mid::Cast &instruction) override;

    void visit(mid::End &instruction) override;

    void visit(mid::Call &instruction) override;

    void visit(mid::Goto &instruction) override;

    void visit(mid::If &instruction) override;

    void visit(mid::Store &constant) override;

    [[nodiscard]] auto &output() const { return _assembly.output(); }

private:
    void _new_function(mid::Function &function);

    void _preamble();

    void _data_section();

    void _comment_instruction(mid::Instruction &instruction);

    void _convert_int_to_int(const mid::Cast &constant, const type::Integral &from,
                             const type::Integral &to);

    void _convert_int_to_float(const mid::Cast &constant, const type::Integral &from,
                               const type::Floating &to);

    void _convert_int_to_bool(const mid::Cast &instruction, const type::Integral &from,
                              const type::Boolean &to);

    void _convert_float_to_float(const mid::Cast &instruction, const type::Floating &from,
                                 const type::Floating &to);

    void _convert_float_to_int(const mid::Cast &instruction, const type::Floating &from,
                               const type::Integral &to);

    void _convert_float_to_bool(const mid::Cast &instruction, const type::Floating &from,
                                const type::Boolean &to);

    void _convert_bool_to_int(const mid::Cast &instruction, const type::Boolean &from,
                              const type::Integral &to);

    void _convert_bool_to_float(const mid::Cast &instruction, const type::Boolean &from,
                                const type::Floating &to);

    Operand _integer_promote(const type::Integral &type, const Operand &operand);

    void _mov(const Type &type, const Operand &destination, const Operand &src);

    void _add(const Type &type, const Operand &destination, const Operand &src);

    void _sub(const Type &type, const Operand &destination, const Operand &src);

    void _div(const Type &type, const Operand &destination, const Operand &src);

    void _mul(const Type &type, const Operand &destination, const Operand &src);

    [[nodiscard]] Register _move_to_temp1(const Type &type, const Operand &src);

    [[nodiscard]] Register _move_to_temp2(const Type &type, const Operand &src);

    [[nodiscard]] static Register _select_register(const Type &type, Register::Base integer, Register::Base floating);

    [[nodiscard]] static Register _returning_register(const Type &type);

    [[nodiscard]] static Register _temp1_register(const Type &type);

    [[nodiscard]] static Register _temp2_register(const Type &type);

private:
    std::unordered_map<mid::Constant, std::string> _constants{};
    OperandResolver _resolver;
    Assembly _assembly{};
};

} // namespace arkoi::back::x86_64