#ifndef ARKOI_LANGUAGE_IL_PRINTER_H
#define ARKOI_LANGUAGE_IL_PRINTER_H

#include "visitor.h"

class ILPrinter : public InstructionVisitor {
public:
    void visit(const LabelInstruction &node) override;

    void visit(const ReturnInstruction &node) override;

    void visit(const BinaryInstruction &node) override;
};

#endif //ARKOI_LANGUAGE_IL_PRINTER_H
