//
// Created by timo on 8/18/24.
//

#ifndef ARKOI_LANGUAGE_TAC_H
#define ARKOI_LANGUAGE_TAC_H

#include <optional>
#include <variant>
#include <memory>
#include <string>

#include "symbol_table.h"

using Operand = std::variant<std::shared_ptr<Symbol>, long long>;

struct TACLabel {
    std::string name;
};

std::ostream &operator<<(std::ostream &os, const TACLabel &instruction);

struct TACReturn {
    std::optional<Operand> value;
};

std::ostream &operator<<(std::ostream &os, const TACReturn &label);

using Instruction = std::variant<TACLabel, TACReturn>;

#endif //ARKOI_LANGUAGE_TAC_H
