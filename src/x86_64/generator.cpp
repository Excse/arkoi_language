#include "x86_64/generator.hpp"

#include "utils/utils.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static const Register::Base TEMP_INT = Register::Base::R10;
static const Register::Base TEMP_SSE = Register::Base::XMM10;

std::stringstream Generator::generate(il::Module &module) {
    std::stringstream output;

    Generator generator;
    generator.visit(module);

    output << generator._text.rdbuf();
    output << generator._data.rdbuf();

    return output;
}

void Generator::visit(il::Module &module) {
    _text << ".intel_syntax noprefix\n";
    _text << ".section .text\n";
    _text << ".global _start\n";
    _text << "\n";

    _text << "_start:\n";
    _text << "\tcall main\n";
    _text << "\tmov rdi, rax\n";
    _text << "\tmov rax, 60\n";
    _text << "\tsyscall\n";
    _text << "\n";

    _data << ".section .data\n";

    for (auto &function: module) {
        function.accept(*this);
    }
}

void Generator::visit(il::Function &function) {
    _mapper = Mapper::map(function);
    _current_function = &function;

    _text << function.name() << ":\n";

    for (auto &block: function) {
        block.accept(*this);
    }
}

void Generator::visit(il::BasicBlock &block) {
    if (_current_function->entry() == &block) {
        _text << "\tenter " << _mapper.stack_size() << ", 0\n";
    } else {
        _text << block.label() << ":\n";
    }

    for (auto &instruction: block) {
        if (std::holds_alternative<il::Alloca>(instruction)) continue;

        _text << "\t# ";
        instruction.accept(_printer);
        _text << "\n";

        instruction.accept(*this);
    }

    if (_current_function->exit() == &block) {
        _text << "\tleave\n";
        _text << "\tret\n";

        _text << "\n";
    }
}

void Generator::visit(il::Return &instruction) {
    // The destination is always either XMM0 or RAX, depending on the return type. Since the mapper already handles this
    // assignment, this instruction is simply translated into a mov instruction.

    auto destination = _load(instruction.result());
    auto source = _load(instruction.value());
    auto &type = instruction.result().type();
    _store(source, destination, type);
}

void Generator::visit(il::Binary &instruction) {
    auto &type = instruction.result().type();
    auto destination = _load(instruction.result());
    auto left = _load(instruction.left());
    auto right = _load(instruction.right());

    Operand source;
    switch (instruction.op()) {
        case il::Binary::Operator::Add: {
            source = _add(left, right, type);
            break;
        }
        case il::Binary::Operator::Sub: {
            source = _sub(left, right, type);
            break;
        }
        case il::Binary::Operator::Mul: {
            source = _mul(left, right, type);
            break;
        }
        case il::Binary::Operator::Div: {
            source = _div(left, right, type);
            break;
        }
        default: {
            source = left;
            break;
        }
    }

    _store(source, destination, type);
}

Operand Generator::_add(Operand left, Operand right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // If the left:right is not of reg:reg or reg:mem the operands need to be adjusted.
        if (!_is_rm(left, right)) {
            // As there are no direct floating immediates (they will always be replaced with memory operands,
            // see _load), we just need to adjust the lhs.
            _adjust_to_r(left, type);
        }

        // Depending on the size of the type choose either addsd or addss.
        if (type.size() == Size::QWORD) {
            _text << "\taddsd";
        } else {
            _text << "\taddss";
        }
        _text << " " << left << ", " << right << "\n";

        // The return operand is the lhs and thus must be returned.
        return left;
    } else {
        // If the left:right is not of reg:mem, reg:reg, mem:reg, reg:imm or mem:imm the operands need to be adjusted.
        if (!(_is_rm(left, right) || _is_mr(left, right) || _is_mi(left, right))) {
            // Here the only combinations are imm:imm, imm:reg or imm:mem, thus just the left operand will be adjusted.
            if (_is_m(right)) {
                // If it is the case of imm:reg or imm:mem, the operands can be swapped as the commutative property of
                // addition allows it.
                std::swap(left, right);
            } else {
                // The only case here is imm:imm, thus it must be transformed into reg:imm
                _adjust_to_r(left, type);
            }
        }

        _text << "\tadd " << left << ", " << right << "\n";

        // The return operand is the lhs and thus must be returned.
        return left;
    }
}

Operand Generator::_sub(Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // If the left:right is not of reg:reg or reg:mem the operands need to be adjusted.
        if (!_is_rm(left, right)) {
            // As there are no direct floating immediates (they will always be replaced with memory operands,
            // see _load), we just need to adjust the lhs.
            _adjust_to_r(left, type);
        }

        // Depending on the size of the type choose either addsd or addss.
        if (type.size() == Size::QWORD) {
            _text << "\tsubsd";
        } else {
            _text << "\tsubss";
        }
        _text << " " << left << ", " << right << "\n";

        // The return operand is the lhs and thus must be returned.
        return left;
    } else {
        // If the left:right is not of reg:mem, reg:reg, mem:reg, reg:imm or mem:imm the operands need to be adjusted.
        if (!(_is_rm(left, right) || _is_mr(left, right) || _is_mi(left, right))) {
            // Here the only combinations are imm:imm, imm:reg or imm:mem, thus just the left operand will be adjusted.
            // As the subtraction doesn't have a commutative property, we can only adjust the lhs to a register.
            // Thus, it will result in reg:imm, reg:reg, reg:mem, which is valid again.
            _adjust_to_r(left, type);
        }

        _text << "\tsub " << left << ", " << right << "\n";

        // The return operand is the lhs and thus must be returned.
        return left;
    }
}

Operand Generator::_mul(const Operand &left, const Operand &, const sem::Type &) {
    return left;
}

Operand Generator::_div(const Operand &left, const Operand &, const sem::Type &) {
    return left;
}

void Generator::visit(il::Cast &) {}

void Generator::visit(il::Call &instruction) {
    // instruction.result() is unnecessary because the mapper always assigns the destination to either the XMM0 register
    // for floating-point values or the RDI register for integers.

    _text << "\tcall " << instruction.name() << "\n";

    auto stack_arguments = Mapper::get_stack_parameters(instruction.arguments());
    auto stack_adjust = 8 * stack_arguments.size();
    stack_adjust = Mapper::align_size(stack_adjust);

    if (stack_adjust) {
        _text << "\tadd rsp, " << stack_adjust << "\n";
    }
}

void Generator::visit(il::If &) {}

void Generator::visit(il::Goto &instruction) {
    _text << "\tjmp " << instruction.label() << "\n";
}

void Generator::visit(il::Store &instruction) {
    auto destination = _load(instruction.result());
    auto source = _load(instruction.value());
    auto &type = instruction.result().type();
    _store(source, destination, type);
}

void Generator::visit(il::Load &instruction) {
    auto destination = _load(instruction.result());
    auto source = _load(instruction.value());
    auto &type = instruction.result().type();
    _store(source, destination, type);
}

void Generator::visit(il::Constant &instruction) {
    // This instruction is generally unused if the optimizer runs. However, during a function call, it is retained since
    // the call instruction only accepts IL variables as arguments. Thus, most of the constant variables will be mapped
    // to a register/memory location or stack push according to the specific calling convention.

    auto destination = _load(instruction.result());
    auto source = _load(instruction.value());
    auto &type = instruction.result().type();
    _store(source, destination, type);
}

Operand Generator::_load(const il::Operand &operand) {
    return std::visit(match{
        [&](const il::Immediate &immediate) -> Operand {
            if (std::holds_alternative<double>(immediate)) {
                auto name = "float" + std::to_string(_constants++);
                _data << "\t" << name << ": .double\t" << immediate << "\n";
                return Memory(Size::QWORD, name);
            } else if (std::holds_alternative<float>(immediate)) {
                auto name = "float" + std::to_string(_constants++);
                _data << "\t" << name << ": .float\t" << immediate << "\n";
                return Memory(Size::DWORD, name);
            } else {
                return std::visit([](const auto &value) -> Immediate { return value; }, immediate);
            }
        },
        [&](const il::Variable &variable) -> Operand {
            return _mapper[variable];
        },
    }, operand);
}

void Generator::_store(Operand source, const Operand &destination, const sem::Type &type) {
    // If the destination is the stack, then just push the operand.
    if (std::holds_alternative<StackPush>(destination)) {
        _text << "\tpush " << source << "\n";
        return;
    }

    // If the source and destination is the same, the mov instruction is not needed.
    if (source == destination) return;

    // Since mov instructions only accept operands in the forms (src:dest) reg:mem, mem:reg, imm:reg, or imm:mem,
    // a mem:mem operation must be split into two mov instructions.
    if (std::holds_alternative<Memory>(source) && std::holds_alternative<Memory>(destination)) {
        if (std::holds_alternative<sem::Floating>(type)) {
            auto target = Register(TEMP_SSE, type.size());
            _store(source, target, type);
            source = target;
        } else {
            auto target = Register(TEMP_INT, type.size());
            _store(source, target, type);
            source = target;
        }
    }

    _text << "\tmov";
    if (std::holds_alternative<sem::Floating>(type)) {
        _text << (type.size() == Size::QWORD ? "sd" : "ss");
    }
    _text << " " << destination << ", " << source << "\n";
}

bool Generator::_is_rm(const Operand &first, const Operand &second) {
    return _is_r(first) && _is_m(second);
}

bool Generator::_is_mr(const Operand &first, const Operand &second) {
    return _is_m(first) && _is_r(second);
}

bool Generator::_is_mi(const Operand &first, const Operand &second) {
    return _is_m(first) && _is_i(second);
}

void Generator::_adjust_to_r(Operand &operand, const sem::Type &type) {
    // Check if the operand is already mem/reg, if yes just return and change nothing.
    if (_is_r(operand)) return;

    // Always adjust to a register, as there is no memory mapped for such temporaries (also it's too expensive).
    const auto &reg_base = (std::holds_alternative<sem::Floating>(type) ? TEMP_SSE : TEMP_INT);

    // Store the contents of operand into the register
    auto temp = Register(reg_base, type.size());
    _store(operand, temp, type);

    // Change the operand to the temporary register.
    operand = temp;
}

bool Generator::_is_m(const Operand &operand) {
    return (std::holds_alternative<Register>(operand) || std::holds_alternative<Memory>(operand));
}

bool Generator::_is_r(const Operand &operand) {
    return std::holds_alternative<Register>(operand);
}

bool Generator::_is_i(const Operand &operand) {
    return std::holds_alternative<Immediate>(operand);
}

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
