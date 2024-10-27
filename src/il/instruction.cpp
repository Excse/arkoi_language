#include "il/instruction.hpp"

namespace il {

Binary::Operator Binary::node_to_instruction(node::Binary::Operator op) {
    switch (op) {
        case node::Binary::Operator::Add: return Operator::Add;
        case node::Binary::Operator::Sub: return Operator::Sub;
        case node::Binary::Operator::Mul: return Operator::Mul;
        case node::Binary::Operator::Div: return Operator::Div;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool Store::has_side_effects() const {
    return !std::holds_alternative<symbol::Temporary>(*_result.symbol());
}

}

std::ostream &operator<<(std::ostream &os, const il::Binary::Operator &op) {
    switch (op) {
        case il::Binary::Operator::Add: return os << "add";
        case il::Binary::Operator::Sub: return os << "sub";
        case il::Binary::Operator::Mul: return os << "mul";
        case il::Binary::Operator::Div: return os << "div";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}