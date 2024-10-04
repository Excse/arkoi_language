#pragma once

#include <sstream>

#include "instruction.hpp"
#include "visitor.hpp"

class ILPrinter : InstructionVisitor {
private:
    ILPrinter() = default;

public:
    [[nodiscard]] static ILPrinter print(const std::vector<std::unique_ptr<Instruction>> &instructions);

    [[nodiscard]] static ILPrinter print(Instruction &instruction);

    void visit(LabelInstruction &instruction) override;

    void visit(BeginInstruction &instruction) override;

    void visit(ReturnInstruction &instruction) override;

    void visit(BinaryInstruction &instruction) override;

    void visit(CastInstruction &instruction) override;

    void visit(EndInstruction &node) override;

    void visit(CallInstruction &instruction) override;

    void visit(ArgumentInstruction &instruction) override;

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};