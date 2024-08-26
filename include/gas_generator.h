#ifndef ARKOI_LANGUAGE_GAS_GENERATOR_H
#define ARKOI_LANGUAGE_GAS_GENERATOR_H

#include <sstream>

#include "symbol_table.h"
#include "instruction.h"
#include "visitor.h"

class GASGenerator : public InstructionVisitor {
public:
    GASGenerator();

    void visit(const LabelInstruction &node) override;

    void visit(const BeginInstruction &node) override;

    void visit(const ReturnInstruction &node) override;

    void visit(const BinaryInstruction &node) override;

    void visit(const EndInstruction &node) override;
    
    [[nodiscard]] const auto &output() const { return _output; }

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

private:
    std::stringstream _output;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
