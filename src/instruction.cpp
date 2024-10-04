#include "instruction.hpp"

BinaryInstruction::Operator BinaryInstruction::node_to_instruction(BinaryNode::Operator op) {
    switch (op) {
        case BinaryNode::Operator::Add: return Operator::Add;
        case BinaryNode::Operator::Sub: return Operator::Sub;
        case BinaryNode::Operator::Mul: return Operator::Mul;
        case BinaryNode::Operator::Div: return Operator::Div;
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Operator &op) {
    switch (op) {
        case BinaryInstruction::Operator::Add: return os << "add";
        case BinaryInstruction::Operator::Sub: return os << "sub";
        case BinaryInstruction::Operator::Mul: return os << "mul";
        case BinaryInstruction::Operator::Div: return os << "div";
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}
