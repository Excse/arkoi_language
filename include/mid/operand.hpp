#pragma once

#include <iostream>
#include <variant>
#include <memory>

#include "mid/symbol_table.hpp"
#include "utils/data.hpp"

namespace arkoi::mid {

class Variable {
public:
    Variable(SharedSymbol symbol, size_t version = 0)
        : _version(version), _symbol(std::move(symbol)) {}

    bool operator==(const Variable &rhs) const;

    bool operator!=(const Variable &rhs) const;

    [[nodiscard]] auto version() const { return _version; }

    [[nodiscard]] auto &symbol() const { return _symbol; }

private:
    size_t _version;
    SharedSymbol _symbol;
};

struct Constant : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    [[nodiscard]] Size size() const;
};

struct Operand : std::variant<Constant, Variable> {
    using variant::variant;
};

}

namespace std {

template<>
struct hash<arkoi::mid::Variable> {
    size_t operator()(const arkoi::mid::Variable &variable) const;
};

template<>
struct hash<arkoi::mid::Constant> {
    size_t operator()(const arkoi::mid::Constant &constant) const;
};

template<>
struct hash<arkoi::mid::Operand> {
    size_t operator()(const arkoi::mid::Operand &operand) const;
};

}

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Constant &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Variable &operand);

std::ostream &operator<<(std::ostream &os, const arkoi::mid::Operand &operand);
