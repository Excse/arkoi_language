#include "instruction.h"

BinaryInstruction::Operator BinaryInstruction::node_to_instruction(BinaryNode::Operator op) {
    switch (op) {
        case BinaryNode::Operator::Add: return BinaryInstruction::Operator::Add;
        case BinaryNode::Operator::Sub: return BinaryInstruction::Operator::Sub;
        case BinaryNode::Operator::Mul: return BinaryInstruction::Operator::Mul;
        case BinaryNode::Operator::Div: return BinaryInstruction::Operator::Div;
        default: throw std::invalid_argument("Binary Instruction not implemented.");
    }
}

std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Operator &op) {
    switch (op) {
        case BinaryInstruction::Operator::Add: return os << "add";
        case BinaryInstruction::Operator::Sub: return os << "sub";
        case BinaryInstruction::Operator::Mul: return os << "mul";
        case BinaryInstruction::Operator::Div: return os << "div";
        default: throw std::invalid_argument("Binary Instruction not implemented.");
    }
}
