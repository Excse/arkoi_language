#include "instruction.h"

BinaryInstruction::Type BinaryInstruction::node_to_instruction(BinaryNode::Type type) {
    switch (type) {
        case BinaryNode::Type::Add: return BinaryInstruction::Type::Add;
        case BinaryNode::Type::Sub: return BinaryInstruction::Type::Sub;
        case BinaryNode::Type::Mul: return BinaryInstruction::Type::Mul;
        case BinaryNode::Type::Div: return BinaryInstruction::Type::Div;
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
