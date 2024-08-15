#ifndef ARKOI_LANGUAGE_VISITOR_H
#define ARKOI_LANGUAGE_VISITOR_H

#include <memory>

class SymbolTable;

class Identifier;

class Function;

class Parameter;

class Program;

class Return;

class Number;

class Block;

class Type;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(const Program &node) = 0;

    virtual void visit(const Function &node) = 0;

    virtual void visit(const Type &node) = 0;

    virtual void visit(const Block &node) = 0;

    virtual void visit(const Parameter &node) = 0;

    virtual void visit(const Number &node) = 0;

    virtual void visit(const Return &node) = 0;

    virtual void visit(const Identifier &node) = 0;
};

#endif //ARKOI_LANGUAGE_VISITOR_H
