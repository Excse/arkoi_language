#pragma once

#include <sstream>

#include "operand.h"

class Assembly {
public:
    void cvttsd2si(const Operand &destination, const Operand &src);

    void cvttss2si(const Operand &destination, const Operand &src);

    void cvtss2sd(const Operand &destination, const Operand &src);

    void cvtsd2ss(const Operand &destination, const Operand &src);

    void cvtsi2ss(const Operand &destination, const Operand &src);

    void cvtsi2sd(const Operand &destination, const Operand &src);

    void movsx(const Operand &destination, const Operand &src);

    void movss(const Operand &destination, const Operand &src);

    void movsd(const Operand &destination, const Operand &src);

    void mov(const Operand &destination, const Operand &src);

    void label(const Symbol &symbol);

    void pop(const Operand &destination);

    void push(const Operand &src);

    void ret();

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

    void comment(const std::string &comment);

    void newline();

    [[nodiscard]] auto &output() { return _output; }

    [[nodiscard]] static std::string double_to_hex(double value);

    [[nodiscard]] static std::string float_to_hex(float value);

private:
    std::stringstream _output{};
};