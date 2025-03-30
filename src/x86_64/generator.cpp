#include "x86_64/generator.hpp"

#include "utils/utils.hpp"
#include "il/cfg.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

static const Register::Base TEMP_INT_1 = Register::Base::A;
static const Register::Base TEMP_INT_2 = Register::Base::C;
static const Register::Base TEMP_SSE_1 = Register::Base::XMM0;
static const Register::Base TEMP_SSE_2 = Register::Base::XMM1;

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
    auto stack_size = _mapper.stack_size();

    if (_current_function->entry() == &block) {
        // If we are in a leaf function and the stack size in less or equal to 128 bytes (redzone), we can skip the enter
        // instruction.
        if (!_current_function->is_leaf() || stack_size > 128) {
            _text << "\tenter " << stack_size << ", 0\n";
        }
    } else {
        // Just a normal block.
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
        // If the function is not a leaf or the stack size exceeds 128 bytes, we need to restore the stack using the
        // leave instruction.
        if (!_current_function->is_leaf() || stack_size > 128) {
            _text << "\tleave\n";
        }

        _text << "\tret\n";
        _text << "\n";
    }
}

void Generator::visit(il::Return &instruction) {
    auto &type = _current_function->type();
    auto result = Mapper::return_register(type);
    auto source = _load(instruction.value());
    _store(source, result, type);
}

void Generator::visit(il::Binary &instruction) {
    auto result = _load(instruction.result());
    auto left = _load(instruction.left());
    auto right = _load(instruction.right());
    auto &type = instruction.op_type();

    switch (instruction.op()) {
        case il::Binary::Operator::Add: return _add(result, left, right, type);
        case il::Binary::Operator::Sub: return _sub(result, left, right, type);
        case il::Binary::Operator::Mul: return _mul(result, left, right, type);
        case il::Binary::Operator::Div: return _div(result, left, right, type);
        case il::Binary::Operator::GreaterThan: return _gth(result, left, right, type);
        case il::Binary::Operator::LessThan: return _lth(result, left, right, type);
    }
}

void Generator::_add(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose addsd or addss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tadd" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

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
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose subsd or subss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tsub" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

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
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose mulsd or mulss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tmul" << suffix << " " << left << ", " << right << "\n";

        // Finally store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        // When discarding the upper part of the multiplication result, imul and mul are indistinguisable. Thus, just
        // imul is used as it also is easier to handle.
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
        left = _adjust_to_reg(result, left, type);

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
            right = _store_temp_2(right, type);
        }

        // Depending on the signess of the integral value, we need to choose idiv or div.
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
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose ucomisd or ucomiss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tucomi" << suffix << " " << left << ", " << right << "\n";

        _text << "\tseta " << result << "\n";
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

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
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose ucomisd or ucomiss.
        auto suffix = (type.size() == Size::QWORD) ? "sd" : "ss";
        _text << "\tucomi" << suffix << " " << left << ", " << right << "\n";

        _text << "\tsetb " << result << "\n";
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

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

void Generator::visit(il::Cast &instruction) {
    auto result = _load(instruction.result());
    auto source = _load(instruction.source());
    auto &from = instruction.from();
    auto to = instruction.result().type();

    std::visit(match{
        [&](const sem::Floating &from, const sem::Floating &to) { _float_to_float(result, source, from, to); },
        [&](const sem::Floating &from, const sem::Integral &to) { _float_to_int(result, source, from, to); },
        [&](const sem::Floating &from, const sem::Boolean &to) { _float_to_bool(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Integral &to) { _int_to_int(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Floating &to) { _int_to_float(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Boolean &to) { _int_to_bool(result, source, from, to); },
        [&](const sem::Boolean &from, const sem::Floating &to) { _bool_to_float(result, source, from, to); },
        [&](const sem::Boolean &from, const sem::Integral &to) { _bool_to_int(result, source, from, to); },
        [&](const sem::Boolean &, const sem::Boolean &) { _store(source, result, to); },
    }, from, to);
}

void Generator::_float_to_float(const Operand &result, Operand source, const sem::Floating &from,
                                const sem::Floating &to) {
    // If both are the same, a simple move will fulfill
    if (from == to) {
        _store(source, result, to);
        return;
    }

    // Always adjust the source to a register.
    source = _adjust_to_reg(result, source, from);

    if (from.size() == Size::DWORD && to.size() == Size::QWORD) {
        // Convert a float operand to a double operand.
        _text << "\tcvtss2sd " << source << ", " << source << "\n";
    } else if (from.size() == Size::QWORD && to.size() == Size::DWORD) {
        // Convert a double operand to a float operand.
        _text << "\tcvtsd2ss " << source << ", " << source << "\n";
    }

    _store(source, result, to);
}

void Generator::_int_to_int(const Operand &result, Operand source, const sem::Integral &from,
                            const sem::Integral &to) {
    // If both are the same exact type just store the source in the result.
    if (from == to) {
        _store(source, result, to);
        return;
    }

    if(from.sign() == to.sign() && !from.sign() && from.size() == Size::DWORD && to.size() == Size::QWORD) {
        // This catches the case when you want to transform a 32bit unsigned integer to a 64bit unsigned integer.
        // There is no zero extension of such operand types, as all 32bit operations implicilty zero-extend 32bit to
        // 64bit.

        // Thus just adjust the source operand to a register of from-size (32bit). This will automatically also zero-extend
        // the upper 32bit.
        source = _adjust_to_reg(result, source, from);

        // Just store the adjusted source in the result (can be either of type mem or reg).
        _store(source, result, to);
    } else if(from.sign() == to.sign() && from.sign() && from.size() == Size::DWORD && to.size() == Size::QWORD) {
        // This is another case where a 32bit signed integer is converted to a 64bit signed integer. The standard movsx
        // is not applicable to this case, thus there is the movsxd instruction that covers this case.

        // movsxd only works with reg:mem or reg:reg, thus convert imm to reg.
        if (std::holds_alternative<Immediate>(source)) {
            source = _store_temp_2(source, from);
        }

        // This will create a register of 64bit that will hold the converted operand.
        auto converted_source = _temp_1_register(to);
        _text << "\tmovsxd " << converted_source << ", " << source << "\n";

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    } else if (from.size() >= to.size()) {
        // This case will resolve the problem, when the from-type is greater than the to-type or if they are same-sized
        // but with different signess.

        // First adjust the source to always be a register of the from-type.
        auto adjusted_source = _adjust_to_reg(result, source, from);
        // Then just use the part of the register that is actually needed (the register part of to-size).
        auto sized_source = Register(adjusted_source.base(), to.size());

        // Store the valid sized register in the result (can be either of type mem or reg).
        _store(sized_source, result, to);
    } else if (from.size() < to.size()) {
        // Here only from-types that are smaller than to-types will be proceeded. Depending on the signess of the
        // from-type we first need to zero/sign extend the source.

        // movsx/movzx only works with reg:mem or reg:reg, thus convert imm to reg.
        if (std::holds_alternative<Immediate>(source)) {
            source = _store_temp_2(source, from);
        }

        // This will create a register of to-size that will hold the converted operand.
        auto converted_source = _temp_1_register(to);

        // Either choose the movsx or movzx instruction based on the from-signess.
        auto suffix = from.sign() ? "sx" : "zx";
        _text << "\tmov" << suffix << " " << converted_source << ", " << source << "\n";

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    }
}

void Generator::_float_to_int(const Operand &result, const Operand &source, const sem::Floating &from,
                              const sem::Integral &to) {
    // Get an int register that is at least 32bit big.
    auto temp_size = (to.size() < Size::DWORD) ? Size::DWORD : to.size();
    auto temp_1_int = _temp_1_register(sem::Integral(temp_size, to.sign()));

    // Either use the cvttsd2si/cvttss2si instruction based on the from-size.
    auto suffix = (from.size() == Size::QWORD) ? "sd" : "ss";
    _text << "\tcvtt" << suffix << "2si " << temp_1_int << ", " << source << "\n";

    // Get the correct sized temp register.
    auto temp_sized = _temp_1_register(to);

    // Store the converted operand in the result (can be either of type mem or reg).
    _store(temp_sized, result, to);
}

void Generator::_float_to_bool(const Operand &result, const Operand &source, const sem::Floating &from,
                               const sem::Boolean &to) {
    // We need temp registers to evaluate if it's a bool or not.
    auto temp_2_sse = _temp_2_register(from);
    auto temp_1_int = _temp_1_register(to);
    auto temp_2_int = _temp_2_register(to);

    // Set the temp sse register to 0.0.
    _text << "\txorps " << temp_2_sse << ", " << temp_2_sse << "\n";

    // Compare the source float with 0.0.
    auto suffix = (from.size() == Size::DWORD) ? "ss" : "sd";
    _text << "\tucomi" << suffix << " " << temp_2_sse << ", " << source << "\n";

    // Set the first temp register if it is not equal to zero.
    _text << "\tsetne " << temp_1_int << "\n";
    // Also set the second temp register to check if the number is NaN.
    _text << "\tsetp " << temp_2_int << "\n";
    // After that combine the result to form a result.
    _text << "\tor " << temp_1_int << ", " << temp_2_int << "\n";

    // Finally store the calculated result in the result operand.
    _store(temp_1_int, result, to);
}

void Generator::_int_to_float(const Operand &result, Operand source, const sem::Integral &from,
                              const sem::Floating &to) {
    if (!from.sign() && from.size() == Size::QWORD) {
        // TODO: Converting a unsigned 64bit integer to float is a bit more complex, thus this is excluded for now.
        throw std::runtime_error("This is not implemented yet.");
    }

    // All used instructions only support reg:mem or reg:reg, thus we need to transform imm to reg.
    if (std::holds_alternative<Immediate>(source)) {
        source = _store_temp_1(source, from);
    }

    if (from.size() < Size::DWORD) {
        // cvtsi2ss/cvtsi2sd only supports 32bit/64bit, thus we need to zero/sign-extend the integer to 32bit.

        // The converted source register with a size of 32bit.
        auto converted_source = _temp_1_register(sem::Integral(Size::DWORD, from.sign()));

        // Either choose the movsx or movzx instruction based on the signess.
        auto suffix = from.sign() ? "sx" : "zx";
        _text << "\tmov" << suffix << " " << converted_source << ", " << source << "\n";

        // Assign the converted source to the source operand.
        source = converted_source;
    }

    // A temporary floating register that is needed to hold the converted integer.
    auto temp_1_sse = _temp_1_register(to);

    // Either choose the cvtsi2ss/cvtsi2sd instruction based on the size.
    auto suffix = (to.size() == Size::QWORD) ? "sd" : "ss";
    _text << "\tcvtsi2" << suffix << " " << temp_1_sse << ", " << source << "\n";

    // Finally store the calculated result in the result operand.
    _store(temp_1_sse, result, to);
}

void Generator::_int_to_bool(const Operand &result, Operand source, const sem::Integral &from,
                             const sem::Boolean &to) {
    // If the source holds an immediate, instead turn it into a register.
    if (std::holds_alternative<Immediate>(source)) {
        source = _store_temp_1(source, from);
    }

    auto temp_1_int = _temp_1_register(to);

    // Compare the source integer with 0.
    _text << "\tcmp " << source << ", 0\n";
    // Set the first temp register if it is not equal to zero.
    _text << "\tsetne " << temp_1_int << "\n";

    // Finally store the calculated result in the result operand.
    _store(temp_1_int, result, to);
}

void Generator::_bool_to_float(const Operand &result, Operand source, const sem::Boolean &from,
                               const sem::Floating &to) {
    // If the source holds an immediate, instead turn it into a register.
    if (std::holds_alternative<Immediate>(source)) {
        source = _store_temp_1(source, from);
    }

    // One sse and one int register is needed to transform a bool to a float.
    auto temp_1_int = _temp_1_register(sem::Integral(Size::DWORD, false));
    auto temp_1_sse = _temp_1_register(to);

    // Zero-extend the bool to 32bit.
    _text << "\tmovzx " << temp_1_int << ", " << source << "\n";

    // Convert the 32bit integer to either a float (cvtsi2ss) or double (cvtsi2sd).
    auto suffix = (to.size() == Size::QWORD ? "sd" : "ss");
    _text << "\tcvtsi2" << suffix << " " << temp_1_sse << ", " << temp_1_int << "\n";

    // Finally store the calculated result in the result operand.
    _store(temp_1_sse, result, to);
}

void Generator::_bool_to_int(const Operand &result, Operand source, const sem::Boolean &from,
                             const sem::Integral &to) {
    if(to.size() == Size::BYTE) {
        // If we want to store a boolean in a 8bit integer we do not need to change anything.

        // Thus we will just store the source in the result (can be either of type mem or reg).
        _store(source, result, to);
    } else {
        // Here we will convert from a 8bit boolean to a 16, 32 or 64bit integer. We just need to zero-extend the boolean
        // and then store the result.

        // If the source holds an immediate, instead turn it into a register.
        if (std::holds_alternative<Immediate>(source)) {
            source = _store_temp_1(source, from);
        }

        // This will create a register of to-size that will hold the converted operand.
        auto converted_source = _temp_1_register(to);

        // Zero-extend the source to the right size.
        _text << "\tmovzx " << converted_source << ", " << source << "\n";

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    }
}

void Generator::visit(il::Call &instruction) {
    auto result = _load(instruction.result());
    auto type = instruction.result().type();

    auto stack_size = _generate_arguments(instruction.arguments());

    _text << "\tcall " << instruction.name() << "\n";

    // We need to clean up the stack if there were some stack arguments.
    if (stack_size != 0) {
        _text << "\tadd rsp, " << stack_size << "\n";
    }

    auto return_reg = Mapper::return_register(type);
    _store(return_reg, result, type);
}

size_t Generator::_generate_arguments(const std::vector<il::Operand> &arguments) {
    size_t integer = 0, floating = 0;

    // The first iteration is needed to calculate the stack arguments to allocate enough stack.
    size_t arg_stack_size = 0;
    for (auto &argument: arguments) {
        const auto &type = argument.type();

        if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
            if (integer++ < INTEGER_ARGUMENT_REGISTERS.size()) continue;
        } else if (std::holds_alternative<sem::Floating>(type)) {
            if (floating++ < SSE_ARGUMENT_REGISTERS.size()) continue;
        }

        arg_stack_size += 8;
    }

    // Align the argument stack size to 16 bytes, so the stack alignment is always fulfilled.
    arg_stack_size = Mapper::align_size(arg_stack_size);

    // If there are stack arguments, make room for them.
    if(arg_stack_size != 0) {
        _text << "\tsub rsp, " << arg_stack_size << "\n";
    }

    // Reset the integer and floating counter.
    integer = 0, floating = 0;
    size_t stack = 0;

    for (auto &argument: arguments) {
        const auto &type = argument.type();
        auto source = _load(argument);

        if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
            if (integer < INTEGER_ARGUMENT_REGISTERS.size()) {
                auto destination = Register(INTEGER_ARGUMENT_REGISTERS[integer], type.size());
                _store(source, destination, type);
                integer++;
                continue;
            }
        } else if (std::holds_alternative<sem::Floating>(type)) {
            if (floating < SSE_ARGUMENT_REGISTERS.size()) {
                auto destination = Register(SSE_ARGUMENT_REGISTERS[floating], type.size());
                _store(source, destination, type);
                floating++;
                continue;
            }
        }

        auto offset = (int64_t) (8 * stack);
        auto memory = Memory(type.size(), RSP, offset);
        _store(source, memory, type);
        stack++;
    }

    return arg_stack_size;
}

void Generator::visit(il::If &instruction) {
    auto condition = _load(instruction.condition());

    // The test instruction only works with reg:imm, mem:imm, reg:reg, mem:reg, thus we simply put the condition in a
    // register if not already available.
    if (!std::holds_alternative<Register>(condition)) {
        condition = _store_temp_1(condition, sem::Boolean());
    }

    _text << "\ttest " << condition << ", " << condition << "\n";
    _text << "\tjnz " << instruction.branch() << "\n";
    _text << "\tjmp " << instruction.next() << "\n";
}

void Generator::visit(il::Goto &instruction) {
    _text << "\tjmp " << instruction.label() << "\n";
}

void Generator::visit(il::Store &instruction) {
    auto result = _load(instruction.result());
    auto source = _load(instruction.source());
    auto type = instruction.result().type();
    _store(source, result, type);
}

void Generator::visit(il::Load &instruction) {
    auto result = _load(instruction.result());
    auto source = _load(instruction.source());
    auto type = instruction.result().type();
    _store(source, result, type);
}

void Generator::visit(il::Constant &instruction) {
    auto result = _load(instruction.result());
    auto immediate = _load(instruction.immediate());
    auto type = instruction.result().type();
    _store(immediate, result, type);
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
        [&](const il::Memory &memory) -> Operand {
            return _mapper[memory];
        },
        [&](const il::Variable &variable) -> Operand {
            return _mapper[variable];
        },
    }, operand);
}

void Generator::_store(Operand source, const Operand &destination, const sem::Type &type) {
    // If the source and destination is the same, the mov instruction is not needed.
    if (source == destination) return;

    // Since mov instructions only accept operands in the forms (src:dest) reg:mem, mem:reg, imm:reg, or imm:mem,
    // a mem:mem operation must be split into two mov instructions.
    if (std::holds_alternative<Memory>(source) && std::holds_alternative<Memory>(destination)) {
        source = _store_temp_1(source, type);
    }

    if (std::holds_alternative<sem::Floating>(type)) {
        auto suffix = (type.size() == Size::QWORD ? "sd" : "ss");
        _text << "\tmov" << suffix << " " << destination << ", " << source << "\n";
    } else {
        _text << "\tmov " << destination << ", " << source << "\n";
    }
}

Register Generator::_store_temp_1(const Operand &source, const sem::Type &type) {
    auto temp = _temp_1_register(type);
    _store(source, temp, type);
    return temp;
}

Register Generator::_temp_1_register(const sem::Type &type) {
    const auto &reg_base = (std::holds_alternative<sem::Floating>(type) ? TEMP_SSE_1 : TEMP_INT_1);
    return {reg_base, type.size()};
}

Register Generator::_store_temp_2(const Operand &source, const sem::Type &type) {
    auto temp = _temp_2_register(type);
    _store(source, temp, type);
    return temp;
}

Register Generator::_temp_2_register(const sem::Type &type) {
    const auto &reg_base = (std::holds_alternative<sem::Floating>(type) ? TEMP_SSE_2 : TEMP_INT_2);
    return {reg_base, type.size()};
}

Register Generator::_adjust_to_reg(const Operand &result, const Operand &operand, const sem::Type &type) {
    // If the result operand is already a register, just use it instead.
    if (std::holds_alternative<Register>(result)) {
        if (result == operand) {
            // If both result and operand are the same register, there is nothing more to do.
            return std::get<Register>(operand);
        }

        // If the result operand is a register, then just temporarily store the operand in it.
        _store(operand, result, type);
        return std::get<Register>(result);
    }

    // Otherwise if the result is not a register, store the lhs in a temp register.
    return _store_temp_1(operand, type);
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
