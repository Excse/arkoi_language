#include "instruction.h"

BinaryInstruction::Type BinaryInstruction::node_to_instruction(BinaryNode::Operator type) {
    switch (type) {
        case BinaryNode::Operator::Add: return BinaryInstruction::Type::Add;
        case BinaryNode::Operator::Sub: return BinaryInstruction::Type::Sub;
        case BinaryNode::Operator::Mul: return BinaryInstruction::Type::Mul;
        case BinaryNode::Operator::Div: return BinaryInstruction::Type::Div;
        default: throw std::invalid_argument("Binary Instruction not implemented.");
    }
}

std::ostream &operator<<(std::ostream &os, const BinaryInstruction::Type &type) {
    switch (type) {
        case BinaryInstruction::Type::Add: return os << "add";
        case BinaryInstruction::Type::Sub: return os << "sub";
        case BinaryInstruction::Type::Mul: return os << "mul";
        case BinaryInstruction::Type::Div: return os << "div";
        default: throw std::invalid_argument("Binary Instruction not implemented.");
    }
}
