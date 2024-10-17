#pragma once

#include <sstream>

#include "backend/x86_64/gas_assembly.hpp"
#include "semantic/symbol_table.hpp"
#include "intermediate/instruction.hpp"
#include "intermediate/il_printer.hpp"
#include "utils/visitor.hpp"
#include "intermediate/cfg.hpp"

namespace arkoi::x86_64 {

class GASGenerator : intermediate::InstructionVisitor {
private:
    GASGenerator() = default;

public:
    [[nodiscard]] static GASGenerator generate(std::vector<CFG> &cfgs,
                                               const std::unordered_map<std::string, Immediate> &data);

    void visit(intermediate::LabelInstruction &instruction) override;

    void visit(intermediate::BeginInstruction &instruction) override;

    void visit(intermediate::ReturnInstruction &instruction) override;

    void visit(intermediate::BinaryInstruction &memory) override;

    void visit(intermediate::CastInstruction &instruction) override;

    void visit(intermediate::EndInstruction &instruction) override;

    void visit(intermediate::CallInstruction &instruction) override;

    void visit(intermediate::ArgumentInstruction &instruction) override;

    void visit(intermediate::GotoInstruction &instruction) override;

    void visit(intermediate::IfNotInstruction &instruction) override;

    void visit(intermediate::StoreInstruction &instruction) override;

    [[nodiscard]] auto &output() const { return _assembly.output(); }

private:
    void _preamble();

    void _data_section(const std::unordered_map<std::string, Immediate> &data);

    void _comment_instruction(intermediate::Instruction &instruction);

    void _convert_int_to_int(const intermediate::CastInstruction &instruction, const type::IntegralType &from,
                             const type::IntegralType &to);

    void _convert_int_to_float(const intermediate::CastInstruction &instruction, const type::IntegralType &from,
                               const type::FloatingType &to);

    void _convert_int_to_bool(const intermediate::CastInstruction &instruction, const type::IntegralType &from,
                              const type::BooleanType &to);

    void _convert_float_to_float(const intermediate::CastInstruction &instruction, const type::FloatingType &from,
                                 const type::FloatingType &to);

    void _convert_float_to_int(const intermediate::CastInstruction &instruction, const type::FloatingType &from,
                               const type::IntegralType &to);

    void _convert_float_to_bool(const intermediate::CastInstruction &instruction, const type::FloatingType &from,
                                const type::BooleanType &to);

    void _convert_bool_to_int(const intermediate::CastInstruction &instruction, const type::BooleanType &from,
                              const type::IntegralType &to);

    void _convert_bool_to_float(const intermediate::CastInstruction &instruction, const type::BooleanType &from,
                                const type::FloatingType &to);

    Operand _integer_promote(const type::IntegralType &type, const Operand &operand);

    void _mov(const type::Type &type, const Operand &destination, const Operand &src);

    void _add(const type::Type &type, const Operand &destination, const Operand &src);

    void _sub(const type::Type &type, const Operand &destination, const Operand &src);

    void _div(const type::Type &type, const Operand &destination, const Operand &src);

    void _mul(const type::Type &type, const Operand &destination, const Operand &src);

    [[nodiscard]] Register _move_to_temp1(const type::Type &type, const Operand &src);

    [[nodiscard]] Register _move_to_temp2(const type::Type &type, const Operand &src);

    [[nodiscard]] static Register _select_register(const type::Type &type, Register::Base integer, Register::Base floating);

    [[nodiscard]] static Register _returning_register(const type::Type &type);

    [[nodiscard]] static Register _temp1_register(const type::Type &type);

    [[nodiscard]] static Register _temp2_register(const type::Type &type);

private:
    Assembly _assembly{};
};

}