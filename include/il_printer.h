#ifndef ARKOI_LANGUAGE_IL_PRINTER_H
#define ARKOI_LANGUAGE_IL_PRINTER_H

#include "visitor.h"

class ILPrinter : public InstructionVisitor {
public:
    void visit(LabelInstruction &node) override;

    void visit(BeginInstruction &node) override;

    void visit(ReturnInstruction &node) override;

    void visit(BinaryInstruction &node) override;

    void visit(EndInstruction &node) override;
};

#endif //ARKOI_LANGUAGE_IL_PRINTER_H
