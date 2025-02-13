#pragma once

namespace arkoi::ast {

class Identifier;
class Parameter;
class Floating;
class Function;
class Integer;
class Program;
class Boolean;
class Assign;
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

    virtual void visit(Assign &node) = 0;

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

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
