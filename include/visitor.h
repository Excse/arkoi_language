//
// Created by timo on 8/6/24.
//

#ifndef ARKOI_LANGUAGE_VISITOR_H
#define ARKOI_LANGUAGE_VISITOR_H

class Identifier;

class Function;

class Argument;

class Program;

class Return;

class Number;

class Block;

class Type;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Program &node) = 0;

    virtual void visit(Function &node) = 0;

    virtual void visit(Type &node) = 0;

    virtual void visit(Block &node) = 0;

    virtual void visit(Argument &node) = 0;

    virtual void visit(Number &node) = 0;

    virtual void visit(Return &node) = 0;

    virtual void visit(Identifier &node) = 0;
};

#endif //ARKOI_LANGUAGE_VISITOR_H
