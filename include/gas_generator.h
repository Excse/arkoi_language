#pragma once

#include <sstream>

#include "gas_assembly.h"
#include "symbol_table.h"
#include "instruction.h"
#include "il_printer.h"
#include "visitor.h"

class GASGenerator : InstructionVisitor {
private:
    GASGenerator() = default;

public:
    [[nodiscard]] static GASGenerator generate(const std::vector<std::unique_ptr<Instruction>> &instructions,
                                               const std::unordered_map<std::string, Immediate> &data);

    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &memory) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &instruction) override;

    [[nodiscard]] auto &output() { return _assembly.output(); }

private:
    void _preamble();

    void _data_section(const std::unordered_map<std::string, Immediate> &data);

    void _comment_instruction(Instruction &instruction);

    void _convert_int_to_int(const IntegerType &from, const Operand &expression,
                             const IntegerType &to, const Operand &destination);

    void _convert_float_to_float(const FloatingType &from, const Operand &expression,
                                 const FloatingType &to, const Operand &destination);

    void _convert_int_to_float(const IntegerType &from, const Operand &expression,
                               const FloatingType &to, const Operand &destination);

    void _convert_float_to_int(const FloatingType &from, const Operand &expression,
                               const IntegerType &to, const Operand &destination);

    void _mov(const Type &type, const Operand &destination, const Operand &src);

    void _add(const Type &type, const Operand &destination, const Operand &src);

    void _sub(const Type &type, const Operand &destination, const Operand &src);

    void _div(const Type &type, const Operand &destination, const Operand &src);

    void _mul(const Type &type, const Operand &destination, const Operand &src);

    [[nodiscard]] static Register _select_register(const Type &type, Register::Base integer, Register::Base floating);

    [[nodiscard]] static Register _returning_register(const Type &type);

    [[nodiscard]] static Register _temp1_register(const Type &type);

    [[nodiscard]] static Register _temp2_register(const Type &type);

private:
    Assembly _assembly{};
};