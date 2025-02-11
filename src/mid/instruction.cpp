#include "mid/instruction.hpp"

using namespace arkoi::mid;

Binary::Operator Binary::node_to_instruction(ast::Binary::Operator op) {
    switch (op) {
        case ast::Binary::Operator::Add: return Operator::Add;
        case ast::Binary::Operator::Sub: return Operator::Sub;
        case ast::Binary::Operator::Mul: return Operator::Mul;
        case ast::Binary::Operator::Div: return Operator::Div;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool Store::has_side_effects() const {
    return !std::holds_alternative<symbol::Temporary>(*_result.symbol());
}

void InstructionType::accept(mid::Visitor &visitor) {
    std::visit([&](auto &item) { item.accept(visitor); }, *this);
}

std::ostream &operator<<(std::ostream &os, const Binary::Operator &op) {
    switch (op) {
        case Binary::Operator::Add: return os << "ADD";
        case Binary::Operator::Sub: return os << "SUB";
        case Binary::Operator::Mul: return os << "MUL";
        case Binary::Operator::Div: return os << "DIV";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}
