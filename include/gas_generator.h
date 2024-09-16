#ifndef ARKOI_LANGUAGE_GAS_GENERATOR_H
#define ARKOI_LANGUAGE_GAS_GENERATOR_H

#include <sstream>

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

    [[nodiscard]] auto &output() const { return _output; }

private:
    void _preamble();

    void _cvttsd2si(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _cvttss2si(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _cvtss2sd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _cvtsd2ss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _cvtsi2ss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _cvtsi2sd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _movsx(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _movss(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _movsd(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _mov(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _label(const std::shared_ptr<Symbol> &symbol);

    void _pop(const std::shared_ptr<Operand> &destination);

    void _push(const std::shared_ptr<Operand> &src);

    void _ret();

    void _add(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _sub(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _idiv(const std::shared_ptr<Operand> &dividend);

    void _imul(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

    void _comment_instruction(Instruction &instruction);

    void _newline();

    static std::shared_ptr<Register> _destination_register(const Type &type);

    static std::shared_ptr<Register> _temp1_register(const Type &type);

    static std::shared_ptr<Register> _temp2_register(const Type &type);

private:
    std::stringstream _output{};
    ILPrinter _printer{};
    bool _debug;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
