#ifndef ARKOI_LANGUAGE_IL_PRINTER_H
#define ARKOI_LANGUAGE_IL_PRINTER_H

#include <sstream>

#include "visitor.h"

class ILPrinter : public InstructionVisitor {
public:
    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &node) override;

    void clear();

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};

#endif //ARKOI_LANGUAGE_IL_PRINTER_H
