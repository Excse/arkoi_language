#pragma once

#include <iostream>
#include <variant>
#include <memory>

#include "semantic/symbol_table.hpp"
#include "utils/data.hpp"

namespace il {

struct Constant : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

struct Operand : std::variant<Constant, Symbol> {
    using variant::variant;
};

}

namespace std {

template<>
struct hash<il::Constant> {
    size_t operator()(const il::Constant &constant) const;
};

template<>
struct hash<il::Operand> {
    size_t operator()(const il::Operand &operand) const;
};

}

std::ostream &operator<<(std::ostream &os, const il::Constant &operand);

std::ostream &operator<<(std::ostream &os, const il::Operand &operand);
