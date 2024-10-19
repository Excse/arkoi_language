#pragma once

#include <iostream>
#include <utility>
#include <variant>
#include <cassert>
#include <memory>

#include "semantic/symbol_table.hpp"
#include "utils/data.hpp"

struct Constant : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

struct Operand : std::variant<Constant, Symbol> {
    using variant::variant;
};

std::ostream &operator<<(std::ostream &os, const Constant &operand);

std::ostream &operator<<(std::ostream &os, const Operand &operand);
