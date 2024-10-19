#pragma once

#include <sstream>

#include "backend/x86_64/assembly.hpp"
#include "semantic/symbol_table.hpp"
#include "intermediate/instruction.hpp"
#include "intermediate/printer.hpp"
#include "utils/visitor.hpp"
#include "intermediate/cfg.hpp"

namespace x86_64 {

class Generator : intermediate::Visitor {
private:
    Generator() = default;

public:
    [[nodiscard]] static Generator generate(std::vector<CFG> &cfgs,
                                            const std::unordered_map<std::string, Immediate> &data);

    void visit(intermediate::Label &instruction) override;

    void visit(intermediate::Begin &instruction) override;

    void visit(intermediate::Return &instruction) override;

    void visit(intermediate::Binary &memory) override;

    void visit(intermediate::Cast &instruction) override;

    void visit(intermediate::End &instruction) override;

    void visit(intermediate::Call &instruction) override;

    void visit(intermediate::Argument &instruction) override;

    void visit(intermediate::Goto &instruction) override;

    void visit(intermediate::IfNot &instruction) override;

    void visit(intermediate::Store &instruction) override;

    [[nodiscard]] auto &output() const { return _assembly.output(); }

private:
    void _preamble();

    void _data_section(const std::unordered_map<std::string, Immediate> &data);

    void _comment_instruction(Instruction &instruction);

    void _convert_int_to_int(const intermediate::Cast &instruction, const type::Integral &from,
                             const type::Integral &to);

    void _convert_int_to_float(const intermediate::Cast &instruction, const type::Integral &from,
                               const type::Floating &to);

    void _convert_int_to_bool(const intermediate::Cast &instruction, const type::Integral &from,
                              const type::Boolean &to);

    void _convert_float_to_float(const intermediate::Cast &instruction, const type::Floating &from,
                                 const type::Floating &to);

    void _convert_float_to_int(const intermediate::Cast &instruction, const type::Floating &from,
                               const type::Integral &to);

    void _convert_float_to_bool(const intermediate::Cast &instruction, const type::Floating &from,
                                const type::Boolean &to);

    void _convert_bool_to_int(const intermediate::Cast &instruction, const type::Boolean &from,
                              const type::Integral &to);

    void _convert_bool_to_float(const intermediate::Cast &instruction, const type::Boolean &from,
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
    Assembly _assembly{};
};

}