#pragma once

#include <sstream>

#include "instruction.hpp"
#include "utils/visitor.hpp"
#include "intermediate/cfg.hpp"

class ILPrinter : InstructionVisitor {
private:
    ILPrinter() = default;

public:
    [[nodiscard]] static ILPrinter print(std::vector<CFG> &cfgs);

    [[nodiscard]] static ILPrinter print(Instruction &instruction);

    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &node) override;

    void visit(CallInstruction &instruction) override;

    void visit(ArgumentInstruction &instruction) override;

    void visit(GotoInstruction &instruction) override;

    void visit(IfNotInstruction &instruction) override;

    void visit(StoreInstruction &instruction) override;

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};