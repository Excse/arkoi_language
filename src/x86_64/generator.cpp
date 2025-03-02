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
    auto &type = _current_function->type();
    auto destination = Mapper::return_register(type);
    auto source = _load(instruction.value());
    _store(source, destination, type);
}

void Generator::visit(il::Binary &instruction) {
    auto &type = instruction.op_type();
    auto result = _load(instruction.result());
    auto left = _load(instruction.left());
    auto right = _load(instruction.right());

    switch (instruction.op()) {
        case il::Binary::Operator::Add: return _add(result, left, right, type);
        case il::Binary::Operator::Sub: return _sub(result, left, right, type);
        case il::Binary::Operator::Mul: return _mul(result, left, right, type);
        case il::Binary::Operator::Div: return _div(result, left, right, type);
        case il::Binary::Operator::GreaterThan: return _gth(result, left, right, type);
        case il::Binary::Operator::LessThan: return _lth(result, left, right, type);
    }
}

Operand Generator::_adjust_lhs_to_reg(const Operand &result, const Operand &left, const sem::Type &type) {
    // Always store the lhs operand in a register. This will minimize the code and also is necessary due to possible
    // overwriting of the lhs operand.
    if (std::holds_alternative<Register>(result)) {
        if (result == left) {
            // The requirement of lhs being a register is already satisfied. Also, the result and lhs register are the
            // same, thus the last store will be ignored.
            return left;
        }

        // The result operand is a register, thus just temporarily store the lhs in it. This will also make sure to
        // ignore the last store.
        _store(left, result, type);
        return result;
    }

    // Otherwise if the result is not a register, store the lhs in a temp register.
    return _store_temp(left, type);
}

void Generator::_add(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose addsd or addss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tadd" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_lhs_to_reg(result, left, type);

        _text << "\tadd " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    }
}

void Generator::_sub(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose subsd or subss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tsub" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_lhs_to_reg(result, left, type);

        _text << "\tsub " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    }
}

void Generator::_mul(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose mulsd or mulss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tmul" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_lhs_to_reg(result, left, type);

        _text << "\timul " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    }
}

void Generator::_div(const Operand &result, Operand left, Operand right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose divsd or divss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tdiv" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // First store the lhs operand in the "A" register of the given operand size.
        auto a_reg = Register(Register::Base::A, type.size());
        _store(left, a_reg, type);

        // The div/idiv instruction only accepts mem/reg, thus an immediate must be converted.
        if (std::holds_alternative<Immediate>(right)) {
            right = _store_temp(right, type);
        }

        _text << "\t";
        if (auto *integral = std::get_if<sem::Integral>(&type)) {
            if (integral->sign()) _text << "i";
        }
        _text << "div " << right << "\n";

        _store(a_reg, result, type);
    }
}

void Generator::_gth(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose ucomisd or ucomiss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tucomi" << suffix << " " << left << ", " << right << "\n";

        _text << "\tseta " << result << "\n";
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_lhs_to_reg(result, left, type);

        _text << "\tcmp " << left << ", " << right << "\n";

        if (auto *integral = std::get_if<sem::Integral>(&type)) {
            if (integral->sign()) {
                _text << "\tsetg " << result << "\n";
            } else {
                _text << "\tseta " << result << "\n";
            }
        } else {
            _text << "\tseta " << result << "\n";
        }
    }
}

void Generator::_lth(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_lhs_to_reg(result, left, type);

        // Depending on the size of the type, either choose ucomisd or ucomiss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tucomi" << suffix << " " << left << ", " << right << "\n";

        _text << "\tsetb " << result << "\n";
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_lhs_to_reg(result, left, type);

        _text << "\tcmp " << left << ", " << right << "\n";

        if (auto *integral = std::get_if<sem::Integral>(&type)) {
            if (integral->sign()) {
                _text << "\tsetl " << result << "\n";
            } else {
                _text << "\tsetb " << result << "\n";
            }
        } else {
            _text << "\tsetb " << result << "\n";
        }
    }
}

void Generator::visit(il::Cast &) {}

void Generator::visit(il::Call &instruction) {
    // instruction.result() is unnecessary because the mapper always assigns the destination to either the XMM0 register
    // for floating-point values or the RDI register for integers.

    _text << "\tcall " << instruction.name() << "\n";

    auto stack_arguments = Mapper::get_stack_parameters(instruction.arguments());
    auto stack_adjust = 8 * stack_arguments.size();

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
        source = _store_temp(source, type);
    }

    if (std::holds_alternative<sem::Floating>(type)) {
        auto suffix = (type.size() == Size::QWORD ? "sd" : "ss");
        _text << "\tmov" << suffix << " " << destination << ", " << source << "\n";
    } else {
        _text << "\tmov " << destination << ", " << source << "\n";
    }
}

Register Generator::_store_temp(const Operand &source, const sem::Type &type) {
    // Always adjust to a register, as there is no memory mapped for such temporaries (also it's too expensive).
    const auto &reg_base = (std::holds_alternative<sem::Floating>(type) ? TEMP_SSE : TEMP_INT);

    // Store the contents of operand into the register
    auto temp = Register(reg_base, type.size());
    _store(source, temp, type);

    return temp;
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
