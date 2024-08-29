#ifndef ARKOI_LANGUAGE_IL_PRINTER_H
#define ARKOI_LANGUAGE_IL_PRINTER_H

#include <sstream>

#include "visitor.h"

class ILPrinter : public InstructionVisitor {
public:
    ILPrinter() : _output() {}

    void visit(LabelInstruction &node) override;

    void visit(BeginInstruction &node) override;

    void visit(ReturnInstruction &node) override;

    void visit(BinaryInstruction &node) override;

    void visit(EndInstruction &node) override;

    void clear();

    [[nodiscard]] const auto &output() const { return _output; }

private:
    std::stringstream _output;
};

#endif //ARKOI_LANGUAGE_IL_PRINTER_H
