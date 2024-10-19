#pragma once

#include <sstream>

#include "intermediate/operand.hpp"

namespace x86_64 {

class Assembly {
public:
    void cvttsd2si(const Operand &destination, const Operand &src);

    void cvttss2si(const Operand &destination, const Operand &src);

    void cvtss2sd(const Operand &destination, const Operand &src);

    void cvtsd2ss(const Operand &destination, const Operand &src);

    void cvtsi2ss(const Operand &destination, const Operand &src);

    void cvtsi2sd(const Operand &destination, const Operand &src);

    void movsx(const Operand &destination, const Operand &src);

    void movzx(const Operand &destination, const Operand &src);

    void movsxd(const Operand &destination, const Operand &src);

    void movss(const Operand &destination, const Operand &src);

    void movsd(const Operand &destination, const Operand &src);

    void mov(const Operand &destination, const Operand &src);

    void label(const Symbol &symbol, bool newline = true);

    void jmp(const Operand &destination);

    void je(const Operand &destination);

    void pop(const Operand &destination);

    void push(const Operand &src);

    void ret();

    void ucomiss(const Operand &destination, const Operand &source);

    void ucomisd(const Operand &destination, const Operand &source);

    void pxor(const Operand &destination, const Operand &source);

    void cmp(const Operand &first, const Operand &second);

    void setne(const Operand &destination);

    void add(const Operand &destination, const Operand &src);

    void addsd(const Operand &destination, const Operand &src);

    void addss(const Operand &destination, const Operand &src);

    void sub(const Operand &destination, const Operand &src);

    void subsd(const Operand &destination, const Operand &src);

    void subss(const Operand &destination, const Operand &src);

    void idiv(const Operand &dividend);

    void div(const Operand &dividend);

    void divsd(const Operand &destination, const Operand &src);

    void divss(const Operand &destination, const Operand &src);

    void imul(const Operand &destination, const Operand &src);

    void mulsd(const Operand &destination, const Operand &src);

    void mulss(const Operand &destination, const Operand &src);

    void directive(const std::string &name, const std::vector<std::string> &settings = {});

    void call(const Symbol &symbol);

    void syscall();

    void comment(const std::string &comment);

    void newline();

    [[nodiscard]] auto &output() const { return _output; }

private:
    std::stringstream _output{};
};

}