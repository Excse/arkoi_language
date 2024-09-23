#pragma once

#include <sstream>

#include "gas_assembly.h"
#include "symbol_table.h"
#include "instruction.h"
#include "il_printer.h"
#include "visitor.h"

class GASGenerator : public InstructionVisitor {
public:
    explicit GASGenerator(bool debug = false);

    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &instruction) override;

    [[nodiscard]] auto &output() { return _assembly.output(); }

private:
    void _preamble();

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

    [[nodiscard]] static std::shared_ptr<Register> _select_register(const Type &type, Register::Base integer,
                                                                    Register::Base floating);

    [[nodiscard]] static std::shared_ptr<Register> _returning_register(const Type &type);

    [[nodiscard]] static std::shared_ptr<Register> _temp1_register(const Type &type);

    [[nodiscard]] static std::shared_ptr<Register> _temp2_register(const Type &type);

private:
    ILPrinter _printer{};
    Assembly _assembly{};
    bool _debug;
};