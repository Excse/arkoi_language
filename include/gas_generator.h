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

    void visit(EndInstruction &instruction) override;

    [[nodiscard]] auto &output() const { return _output; }

private:
    void _preamble();

    void _load(const Operand &operand, const std::string &destination);

    void _store(const Operand &operand, const std::string &src);

    void _mov(const std::string &destination, const std::string &src);

    void _label(const std::shared_ptr<Symbol> &symbol);

    void _pop(const std::string &destination);

    void _push(const std::string &src);

    void _ret();

    void _add(const std::string &destination, const std::string &src);

    void _sub(const std::string &destination, const std::string &src);

    void _idiv(const std::string &dividend);

    void _imul(const std::string &destination, const std::string &src);

    void _comment_instruction(Instruction &instruction);

    void _newline();

private:
    std::stringstream _output;
    ILPrinter _printer;
    bool _debug;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
