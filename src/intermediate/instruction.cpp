#include "intermediate/instruction.hpp"

using namespace arkoi::intermediate;

BinaryInstruction::Operator BinaryInstruction::node_to_instruction(arkoi::ast::BinaryNode::Operator op) {
    switch (op) {
        case arkoi::ast::BinaryNode::Operator::Add: return Operator::Add;
        case arkoi::ast::BinaryNode::Operator::Sub: return Operator::Sub;
        case arkoi::ast::BinaryNode::Operator::Mul: return Operator::Mul;
        case arkoi::ast::BinaryNode::Operator::Div: return Operator::Div;
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
