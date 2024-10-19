#include "intermediate/instruction.hpp"

namespace intermediate {

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

}

std::ostream &operator<<(std::ostream &os, const intermediate::Binary::Operator &op) {
    switch (op) {
        using Operator = intermediate::Binary::Operator;
        case Operator::Add: return os << "add";
        case Operator::Sub: return os << "sub";
        case Operator::Mul: return os << "mul";
        case Operator::Div: return os << "div";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}