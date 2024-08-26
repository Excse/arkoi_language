#ifndef ARKOI_LANGUAGE_VISITOR_H
#define ARKOI_LANGUAGE_VISITOR_H

#include <memory>

class SymbolTable;

class IdentifierNode;

class FunctionNode;

class ParameterNode;

class ProgramNode;

class ReturnNode;

class NumberNode;

class BinaryNode;

class BlockNode;

class TypeNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(const ProgramNode &node) = 0;

    virtual void visit(const FunctionNode &node) = 0;

    virtual void visit(const TypeNode &node) = 0;

    virtual void visit(const BlockNode &node) = 0;

    virtual void visit(const ParameterNode &node) = 0;

    virtual void visit(const NumberNode &node) = 0;

    virtual void visit(const ReturnNode &node) = 0;

    virtual void visit(const IdentifierNode &node) = 0;

    virtual void visit(const BinaryNode &node) = 0;
};

class BinaryInstruction;

class ReturnInstruction;

class LabelInstruction;

class BeginInstruction;

class EndInstruction;

class InstructionVisitor {
public:
    virtual ~InstructionVisitor() = default;

    virtual void visit(const LabelInstruction &node) = 0;

    virtual void visit(const BeginInstruction &node) = 0;

    virtual void visit(const ReturnInstruction &node) = 0;

    virtual void visit(const BinaryInstruction &node) = 0;

    virtual void visit(const EndInstruction &node) = 0;
};

#endif //ARKOI_LANGUAGE_VISITOR_H
