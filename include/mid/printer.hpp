#pragma once

#include <sstream>

#include "utils/visitor.hpp"
#include "mid/instruction.hpp"
#include "mid/cfg.hpp"

namespace arkoi::mid {

class Printer : Visitor {
private:
    Printer() = default;

public:
    [[nodiscard]] static Printer print(Module &module);

    void visit(Module &module) override;

    void visit(Function &function) override;

    void visit(BasicBlock &block) override;

    void visit(Label &instruction) override;

    void visit(Return &instruction) override;

    void visit(Binary &instruction) override;

    void visit(Cast &instruction) override;

    void visit(Call &instruction) override;

    void visit(Goto &instruction) override;

    void visit(If &instruction) override;

    void visit(Store &instruction) override;

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};

} // namespace arkoi::mid