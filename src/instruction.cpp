#include "instruction.h"

#include "utils.h"

std::ostream &operator<<(std::ostream &os, const Operand &token) {
    std::visit(match{
            [&](const std::shared_ptr<Symbol> &symbol) { os << *symbol; },
            [&](const auto &item) { os << item; }
    }, token);
    return os;
}

BinaryInstruction::Type BinaryInstruction::node_to_instruction(BinaryNode::Type type) {
    switch (type) {
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

std::string BinaryInstruction::type_to_string(BinaryInstruction::Type type) {
    switch (type) {
        case BinaryInstruction::Type::Add:
            return "+";
        case BinaryInstruction::Type::Sub:
            return "-";
        case BinaryInstruction::Type::Mul:
            return "*";
        case BinaryInstruction::Type::Div:
            return "/";
    }
}
