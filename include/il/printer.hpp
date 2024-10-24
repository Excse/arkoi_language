#pragma once

#include <sstream>

#include "utils/visitor.hpp"
#include "il/instruction.hpp"
#include "il/cfg.hpp"

namespace il {

class Printer : Visitor {
private:
    Printer() = default;

public:
    [[nodiscard]] static Printer print(std::vector<Function> &functions);

    [[nodiscard]] static Printer print(Instruction &instruction);

    void visit(Label &instruction) override;

    void visit(Begin &instruction) override;

    void visit(Return &instruction) override;

    void visit(Binary &instruction) override;

    void visit(Cast &instruction) override;

    void visit(End &node) override;

    void visit(Call &instruction) override;

    void visit(Goto &instruction) override;

    void visit(If &instruction) override;

    void visit(Store &instruction) override;

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};

}