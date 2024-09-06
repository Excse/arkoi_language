#ifndef ARKOI_LANGUAGE_GAS_GENERATOR_H
#define ARKOI_LANGUAGE_GAS_GENERATOR_H

#include <sstream>

#include "symbol_table.h"
#include "instruction.h"
#include "il_printer.h"
#include "visitor.h"

using Source = std::variant<Memory, Register, Immediate>;

std::ostream &operator<<(std::ostream &os, const Source &source);

using Destination = std::variant<Memory, Register>;

std::ostream &operator<<(std::ostream &os, const Destination &destination);

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

    void _movsx(const Destination &destination, const Source &src);

    void _mov(const Destination &destination, const Source &src);

    void _label(const std::shared_ptr<Symbol> &symbol);

    void _pop(const Destination &destination);

    void _push(const Source &src);

    void _ret();

    void _add(const Destination &destination, const Source &src);

    void _sub(const Destination &destination, const Source &src);

    void _idiv(const Source &dividend);

    void _imul(const Destination &destination, const Source &src);

    void _comment_instruction(Instruction &instruction);

    void _newline();

    [[nodiscard]] static Destination _to_destination(const Operand &operand);

    [[nodiscard]] static Source _to_source(const Operand &operand);

private:
    std::stringstream _output{};
    ILPrinter _printer{};
    bool _debug;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
