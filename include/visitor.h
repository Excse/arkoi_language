#pragma once

#include <memory>

class SymbolTable;

class IdentifierNode;

class ParameterNode;

class FloatingNode;

class FunctionNode;

class IntegerNode;

class ProgramNode;

class ReturnNode;

class BinaryNode;

class BlockNode;

class CastNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(ProgramNode &node) = 0;

    virtual void visit(FunctionNode &node) = 0;

    virtual void visit(BlockNode &node) = 0;

    virtual void visit(ParameterNode &node) = 0;

    virtual void visit(IntegerNode &node) = 0;

    virtual void visit(FloatingNode &node) = 0;

    virtual void visit(ReturnNode &node) = 0;

    virtual void visit(IdentifierNode &node) = 0;

    virtual void visit(BinaryNode &node) = 0;

    virtual void visit(CastNode &node) = 0;
};

class BinaryInstruction;

class ReturnInstruction;

class LabelInstruction;

class BeginInstruction;

class CastInstruction;

class EndInstruction;

class InstructionVisitor {
public:
    virtual ~InstructionVisitor() = default;

    virtual void visit(LabelInstruction &instruction) = 0;

    virtual void visit(BeginInstruction &instruction) = 0;

    virtual void visit(ReturnInstruction &instruction) = 0;

    virtual void visit(BinaryInstruction &instruction) = 0;

    virtual void visit(EndInstruction &instruction) = 0;

    virtual void visit(CastInstruction &instruction) = 0;
};