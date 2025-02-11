#pragma once

namespace arkoi::ast {

class Identifier;
class Parameter;
class Floating;
class Function;
class Integer;
class Program;
class Boolean;
class Return;
class Binary;
class Block;
class Cast;
class Call;
class If;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Program &node) = 0;

    virtual void visit(Function &node) = 0;

    virtual void visit(Block &node) = 0;

    virtual void visit(Parameter &node) = 0;

    virtual void visit(Integer &node) = 0;

    virtual void visit(Floating &node) = 0;

    virtual void visit(Boolean &node) = 0;

    virtual void visit(Return &node) = 0;

    virtual void visit(Identifier &node) = 0;

    virtual void visit(Binary &node) = 0;

    virtual void visit(Cast &node) = 0;

    virtual void visit(Call &node) = 0;

    virtual void visit(If &node) = 0;
};

} // namespace arkoi::ast

namespace arkoi::mid {

class BasicBlock;
class Function;
class Module;
class Binary;
class Return;
class Label;
class Store;
class Cast;
class Call;
class Goto;

class If;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Module &module) = 0;

    virtual void visit(Function &function) = 0;

    virtual void visit(BasicBlock &block) = 0;

    virtual void visit(Label &instruction) = 0;

    virtual void visit(Return &instruction) = 0;

    virtual void visit(Binary &instruction) = 0;

    virtual void visit(Cast &instruction) = 0;

    virtual void visit(Call &instruction) = 0;

    virtual void visit(If &instruction) = 0;

    virtual void visit(Goto &instruction) = 0;

    virtual void visit(Store &instruction) = 0;
};

} // namespace arkoi::mid