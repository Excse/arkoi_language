#ifndef ARKOI_LANGUAGE_GAS_GENERATOR_H
#define ARKOI_LANGUAGE_GAS_GENERATOR_H

#include "visitor.h"

class GASGenerator : public InstructionVisitor {
public:
    GASGenerator();

    void visit(const LabelInstruction &node) override;

    void visit(const ReturnInstruction &node) override;

    void visit(const BinaryInstruction &node) override;
    
    [[nodiscard]] const auto &output() const { return _output; }

private:
    void _preamble();

private:
    std::string _output;
};


#endif //ARKOI_LANGUAGE_GAS_GENERATOR_H
