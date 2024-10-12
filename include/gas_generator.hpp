#pragma once

#include <sstream>

#include "gas_assembly.hpp"
#include "symbol_table.hpp"
#include "instruction.hpp"
#include "il_printer.hpp"
#include "visitor.hpp"
#include "cfg.hpp"

class GASGenerator : InstructionVisitor {
private:
    GASGenerator() = default;

public:
    [[nodiscard]] static GASGenerator generate(std::vector<CFG> &cfgs,
                                               const std::unordered_map<std::string, Immediate> &data);

    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &memory) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &instruction) override;

    void visit(CallInstruction &instruction) override;

    void visit(ArgumentInstruction &instruction) override;

    void visit(GotoInstruction &instruction) override;

    void visit(IfNotInstruction &instruction) override;

    void visit(StoreInstruction &instruction) override;

    [[nodiscard]] auto &output() const { return _assembly.output(); }

private:
    void _preamble();

    void _data_section(const std::unordered_map<std::string, Immediate> &data);

    void _comment_instruction(InstructionBase &instruction);

    void _convert_int_to_int(const CastInstruction &instruction, const IntegralType &from, const IntegralType &to);

    void _convert_int_to_float(const CastInstruction &instruction, const IntegralType &from, const FloatingType &to);

    void _convert_int_to_bool(const CastInstruction &instruction, const IntegralType &from, const BooleanType &to);

    void _convert_float_to_float(const CastInstruction &instruction, const FloatingType &from, const FloatingType &to);

    void _convert_float_to_int(const CastInstruction &instruction, const FloatingType &from, const IntegralType &to);

    void _convert_float_to_bool(const CastInstruction &instruction, const FloatingType &from, const BooleanType &to);

    void _convert_bool_to_int(const CastInstruction &instruction, const BooleanType &from, const IntegralType &to);

    void _convert_bool_to_float(const CastInstruction &instruction, const BooleanType &from, const FloatingType &to);

    Operand _integer_promote(const IntegralType &type, const Operand &operand);

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