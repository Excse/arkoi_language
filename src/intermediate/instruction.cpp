#include "intermediate/instruction.hpp"

using namespace arkoi::intermediate;

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

std::ostream &operator<<(std::ostream &os, const Binary::Operator &op) {
    switch (op) {
        case Binary::Operator::Add: return os << "add";
        case Binary::Operator::Sub: return os << "sub";
        case Binary::Operator::Mul: return os << "mul";
        case Binary::Operator::Div: return os << "div";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}
