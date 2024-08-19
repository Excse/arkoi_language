#include "tac.h"

BinaryInstruction::Type BinaryInstruction::node_to_instruction(BinaryNode::Type type) {
    switch(type) {
        case BinaryNode::Type::Add:
            return BinaryInstruction::Type::Add;
        case BinaryNode::Type::Sub:
            return BinaryInstruction::Type::Sub;
        case BinaryNode::Type::Mul:
            return BinaryInstruction::Type::Mul;
        case BinaryNode::Type::Div:
            return BinaryInstruction::Type::Div;
    }
}