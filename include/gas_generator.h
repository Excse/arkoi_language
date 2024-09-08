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

    void _movsx(const std::shared_ptr<Operand> &destination, const std::shared_ptr<Operand> &src);

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

private:
    std::stringstream _output{};
    ILPrinter _printer{};
    bool _debug;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
