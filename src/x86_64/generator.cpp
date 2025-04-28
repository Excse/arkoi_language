#include "x86_64/generator.hpp"

#include "il/cfg.hpp"
#include "il/il_printer.hpp"
#include "utils/utils.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

std::stringstream Generator::generate(il::Module &module) {
    std::stringstream output;

    Generator generator;
    generator.visit(module);

    for (const auto &item: generator._text) output << item << "\n";
    for (const auto &item: generator._data) output << item << "\n";

    return output;
}

void Generator::visit(il::Module &module) {
    _directive(".section .data", _data);

    _directive(".intel_syntax noprefix", _text);
    _directive(".section .text", _text);
    _directive(".global _start", _text);
    _newline(_text);

    _label("_start");
    _call("main");
    _mov(RDI, RAX);
    _mov(RAX, 60);
    _syscall();
    _newline(_text);

    for (auto &function: module) {
        function.accept(*this);
    }
}

void Generator::visit(il::Function &function) {
    _current_mapper = Mapper::map(function);
    _current_function = &function;

    _label(function.name());

    for (auto &block: function) {
        block.accept(*this);
    }
}

void Generator::visit(il::BasicBlock &block) {
    const auto stack_size = _current_mapper.stack_size();

    if (_current_function->entry() == &block) {
        // If we are in a leaf function and the stack size in less or equal to 128 bytes (redzone), we can skip the enter
        // instruction.
        if (!_current_function->is_leaf() || stack_size > 128) _enter(stack_size, 0);
    } else {
        // Just a normal block.
        _label(block.label());
    }

    for (auto &instruction: block) {
        if (std::holds_alternative<il::Alloca>(instruction)) continue;

        std::stringstream output;
        output << "\t# ";
        il::ILPrinter printer(output);
        instruction.accept(printer);

        _directive(output.str(), _text);
        instruction.accept(*this);
    }

    if (_current_function->exit() == &block) {
        // If the function is not a leaf or the stack size exceeds 128 bytes, we need to restore the stack using the
        // leave instruction.
        if (!_current_function->is_leaf() || stack_size > 128) _leave();

        _ret();
        _newline(_text);
    }
}

void Generator::visit(il::Binary &instruction) {
    const auto result = _load(instruction.result());
    const auto left = _load(instruction.left());
    const auto right = _load(instruction.right());
    const auto &type = instruction.op_type();

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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_addsd : &Generator::_addss;
        (this->*instruction)(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register, you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        _add(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_subsd : &Generator::_subss;
        (this->*instruction)(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register, you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        _sub(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_mulsd : &Generator::_mulss;
        (this->*instruction)(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
        _store(left, result, type);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register, you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        // When discarding the upper part of the multiplication result, imul and mul are indistinguisable. Thus, just
        // imul is used as it also is easier to handle.
        _imul(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_divsd : &Generator::_divss;
        (this->*instruction)(left, right);

        // Finally, store the lhs (where the result is written to) to the result operand.
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
        auto *integral = std::get_if<sem::Integral>(&type);
        const auto &instruction = (integral && integral->sign()) ? &Generator::_idiv : &Generator::_udiv;
        (this->*instruction)(right);

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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_ucomisd : &Generator::_ucomiss;
        (this->*instruction)(left, right);

        _seta(result);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register, you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        _cmp(left, right);

        const auto *integral = std::get_if<sem::Integral>(&type);
        const auto &instruction = (integral && integral->sign()) ? &Generator::_setg : &Generator::_seta;
        (this->*instruction)(result);
    }
}

void Generator::_lth(const Operand &result, Operand left, const Operand &right, const sem::Type &type) {
    if (std::holds_alternative<sem::Floating>(type)) {
        // As there are no direct floating immediates (they will always be replaced with memory operands, see _load),
        // we just need to adjust the lhs to a register, which we always do.
        // Thus left:right will always be reg:mem or reg:reg, which is a valid operand encoding.
        left = _adjust_to_reg(result, left, type);

        // Depending on the size of the type, either choose ucomisd or ucomiss.
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_ucomisd : &Generator::_ucomiss;
        (this->*instruction)(left, right);

        _setb(result);
    } else {
        // The only valid combinations of left:right are reg:mem, reg:reg, mem:reg, reg:imm, mem:imm. But as left will
        // always be a register, you only need to care about reg:mem, reg:reg, reg:imm, which covers all other cases.
        left = _adjust_to_reg(result, left, type);

        _cmp(left, right);

        const auto *integral = std::get_if<sem::Integral>(&type);
        const auto &instruction = (integral && integral->sign()) ? &Generator::_setl : &Generator::_setb;
        (this->*instruction)(result);
    }
}

void Generator::visit(il::Cast &instruction) {
    const auto result = _load(instruction.result());
    const auto source = _load(instruction.source());

    std::visit(match{
        [&](const sem::Floating &from, const sem::Floating &to) { _float_to_float(result, source, from, to); },
        [&](const sem::Floating &from, const sem::Integral &to) { _float_to_int(result, source, from, to); },
        [&](const sem::Floating &from, const sem::Boolean &to) { _float_to_bool(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Integral &to) { _int_to_int(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Floating &to) { _int_to_float(result, source, from, to); },
        [&](const sem::Integral &from, const sem::Boolean &to) { _int_to_bool(result, source, from, to); },
        [&](const sem::Boolean &from, const sem::Floating &to) { _bool_to_float(result, source, from, to); },
        [&](const sem::Boolean &from, const sem::Integral &to) { _bool_to_int(result, source, from, to); },
        [&](const sem::Boolean &, const sem::Boolean &to) { _store(source, result, to); },
    }, instruction.from(), instruction.result().type());
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

    auto converted_source = std::get<Register>(source);
    converted_source.set_size(to.size());

    const auto &instruction = (from.size() == Size::QWORD) ? &Generator::_cvtsd2ss : &Generator::_cvtss2sd;
    (this->*instruction)(converted_source, source);

    _store(converted_source, result, to);
}

void Generator::_int_to_int(const Operand &result, Operand source, const sem::Integral &from,
                            const sem::Integral &to) {
    // If both are the same exact type, store the source in the result.
    if (from == to) {
        _store(source, result, to);
        return;
    }

    if(from.sign() == to.sign() && !from.sign() && from.size() == Size::DWORD && to.size() == Size::QWORD) {
        // This catches the case when you want to transform a 32bit unsigned integer to a 64bit unsigned integer.
        // There is no zero extension of such operand types, as all 32bit operations implicilty zero-extend 32bit to
        // 64bit.

        // Thus adjust the source operand to a register of from-size (32bit). This will automatically also zero-extend
        // the upper 32bit.
        source = _adjust_to_reg(result, source, from);

        // Store the adjusted source in the result (can be either of type mem or reg).
        _store(source, result, to);
    } else if(from.sign() == to.sign() && from.sign() && from.size() == Size::DWORD && to.size() == Size::QWORD) {
        // This is another case where a 32bit signed integer is converted to a 64bit signed integer. The standard movsx
        // is not applicable to this case, thus there is the movsxd instruction that covers this case.

        // movsxd only works with reg:mem or reg:reg, thus convert imm to reg.
        if (std::holds_alternative<Immediate>(source)) {
            source = _adjust_to_reg(result, source, from);
        }

        // This will create a register of 64bit that will hold the converted operand.
        auto converted_source = _temp_1_register(to);
        _movsxd(converted_source, source);

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    } else if (from.size() >= to.size()) {
        // This case will resolve the problem when the from-type is greater than the to-type or if they are same-sized
        // but with different signess.

        // First, adjust the source to always be a register of the from-type.
        const auto adjusted_source = _adjust_to_reg(result, source, from);
        // Then use the part of the register that is actually needed (the register part of to-size).
        auto sized_source = Register(adjusted_source.base(), to.size());

        // Store the valid sized register in the result (can be either of type mem or reg).
        _store(sized_source, result, to);
    } else if (from.size() < to.size()) {
        // Here only from-types that are smaller than to-types will be proceeded. Depending on the signess of the
        // from-type, we first need to zero/sign extend the source.

        // movsx/movzx only works with reg:mem or reg:reg, thus convert imm to reg.
        if (std::holds_alternative<Immediate>(source)) {
            source = _adjust_to_reg(result, source, from);
        }

        // This will create a register of to-size that will hold the converted operand.
        auto converted_source = _temp_1_register(to);

        // Either choose the movsx or movzx instruction based on the from-signess.
        const auto &instruction = from.sign() ? &Generator::_movsx : &Generator::_movzx;
        (this->*instruction)(converted_source, source);

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    }
}

void Generator::_float_to_int(const Operand &result, const Operand &source, const sem::Floating &from,
                              const sem::Integral &to) {
    // Get an int register that is at least 32bit big.
    const auto temp_size = (to.size() < Size::DWORD) ? Size::DWORD : to.size();
    const auto temp_1_int = _temp_1_register(sem::Integral(temp_size, to.sign()));

    // Either use the cvttsd2si/cvttss2si instruction based on the from-size.
    const auto &instruction = (from.size() == Size::QWORD) ? &Generator::_cvttsd2si : &Generator::_cvttss2si;
    (this->*instruction)(temp_1_int, source);

    // Get the correct sized temp register.
    auto temp_sized = _temp_1_register(to);

    // Store the converted operand in the result (can be either of type mem or reg).
    _store(temp_sized, result, to);
}

void Generator::_float_to_bool(const Operand &result, const Operand &source, const sem::Floating &from,
                               const sem::Boolean &to) {
    // We need temp registers to evaluate if it's a bool or not.
    const auto temp_2_sse = _temp_2_register(from);
    const auto temp_1_int = _temp_1_register(to);
    const auto temp_2_int = _temp_2_register(to);

    // Set the temp sse register to 0.0.
    _xorps(temp_2_sse, temp_2_sse);

    // Compare the source float with 0.0.
    const auto &instruction = (from.size() == Size::QWORD) ? &Generator::_ucomisd : &Generator::_ucomiss;
    (this->*instruction)(temp_2_sse, source);

    // Set the first temp register if it is not equal to zero.
    _setne(temp_1_int);
    // Also, set the second temp register to check if the number is NaN.
    _setp(temp_2_int);
    // After that combine the result to form a result.
    _or(temp_1_int, temp_2_int);

    // Finally, store the calculated result in the result operand.
    _store(temp_1_int, result, to);
}

void Generator::_int_to_float(const Operand &result, Operand source, const sem::Integral &from,
                              const sem::Floating &to) {
    if (!from.sign() && from.size() == Size::QWORD) {
        // TODO(timo): Converting a unsigned 64bit integer to float is a bit more complex, thus this is excluded for now.
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
        const auto &instruction = from.sign() ? &Generator::_movsx : &Generator::_movzx;
        (this->*instruction)(converted_source, source);

        // Assign the converted source to the source operand.
        source = converted_source;
    }

    // A temporary floating register that is needed to hold the converted integer.
    auto temp_1_sse = _temp_1_register(to);

    // Either choose the cvtsi2ss/cvtsi2sd instruction based on the size.
    const auto &instruction = (to.size() == Size::QWORD) ? &Generator::_cvtsi2sd : &Generator::_cvtsi2ss;
    (this->*instruction)(temp_1_sse, source);

    // Finally, store the calculated result in the result operand.
    _store(temp_1_sse, result, to);
}

void Generator::_int_to_bool(const Operand &result, Operand source, const sem::Integral &from,
                             const sem::Boolean &to) {
    // If the source holds an immediate, instead turn it into a register.
    if (std::holds_alternative<Immediate>(source)) {
        source = _adjust_to_reg(result, source, from);
    }

    const auto temp_1_int = _temp_1_register(to);

    // Compare the source integer with 0.
    _cmp(source, 0);
    // Set the first temp register if it is not equal to zero.
    _setne(temp_1_int);

    // Finally, store the calculated result in the result operand.
    _store(temp_1_int, result, to);
}

void Generator::_bool_to_float(const Operand &result, Operand source, const sem::Boolean &from,
                               const sem::Floating &to) {
    // If the source holds an immediate, instead turn it into a register.
    if (std::holds_alternative<Immediate>(source)) {
        source = _store_temp_1(source, from);
    }

    // One sse and one int register is needed to transform a bool to a float.
    const auto temp_1_int = _temp_1_register(sem::Integral(Size::DWORD, false));
    const auto temp_1_sse = _temp_1_register(to);

    // Zero-extend the bool to 32bit.
    _movzx(temp_1_int, source);

    // Convert the 32bit integer to either a float (cvtsi2ss) or double (cvtsi2sd).
    const auto &instruction = (to.size() == Size::QWORD) ? &Generator::_cvtsi2sd : &Generator::_cvtsi2ss;
    (this->*instruction)(temp_1_sse, temp_1_int);

    // Finally, store the calculated result in the result operand.
    _store(temp_1_sse, result, to);
}

void Generator::_bool_to_int(const Operand &result, Operand source, const sem::Boolean &from,
                             const sem::Integral &to) {
    if(to.size() == Size::BYTE) {
        // If we want to store a boolean in a 8bit integer we do not need to change anything.

        // Thus we will just store the source in the result (can be either of type mem or reg).
        _store(source, result, to);
    } else {
        // Here we will convert from 8bit boolean to a 16, 32 or 64bit integer. We just need to zero-extend the boolean
        // and then store the result.

        // If the source holds an immediate, instead turn it into a register.
        if (std::holds_alternative<Immediate>(source)) {
            source = _adjust_to_reg(result, source, from);
        }

        // This will create a register of to-size that will hold the converted operand.
        auto converted_source = _temp_1_register(to);

        // Zero-extend the source to the right size.
        _movzx(converted_source, source);

        // Store the converted operand in the result (can be either of type mem or reg).
        _store(converted_source, result, to);
    }
}

void Generator::visit(il::Call &instruction) {
    const auto result = _load(instruction.result());
    const auto type = instruction.result().type();

    const auto stack_size = _generate_arguments(instruction.arguments());

    _call(instruction.name());

    // We need to clean up the stack if there were some stack arguments.
    if (stack_size != 0) _add(RSP, stack_size);

    const auto return_reg = Mapper::return_register(type);
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
    if (arg_stack_size != 0) _sub(RSP, arg_stack_size);

    // Reset the integer and floating counter.
    integer = 0, floating = 0;
    size_t stack = 0;

    for (auto &argument: arguments) {
        const auto &type = argument.type();
        const auto source = _load(argument);

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

        const auto offset = static_cast<int64_t>(8 * stack);
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

    _test(condition, condition);
    _jnz(instruction.branch());
    _jmp(instruction.next());
}

void Generator::visit(il::Goto &instruction) {
    _jmp(instruction.label());
}

void Generator::visit(il::Store &instruction) {
    const auto result = _load(instruction.result());
    const auto source = _load(instruction.source());
    const auto type = instruction.result().type();
    _store(source, result, type);
}

void Generator::visit(il::Load &instruction) {
    const auto result = _load(instruction.result());
    const auto source = _load(instruction.source());
    const auto type = instruction.result().type();
    _store(source, result, type);
}

void Generator::visit(il::Constant &instruction) {
    const auto result = _load(instruction.result());
    const auto immediate = _load(instruction.immediate());
    const auto type = instruction.result().type();
    _store(immediate, result, type);
}

Operand Generator::_load(const il::Operand &operand) {
    return std::visit(match{
        [&](const il::Immediate &immediate) -> Operand {
            if (std::holds_alternative<double>(immediate)) {
                const auto name = "float" + std::to_string(_constants++);
                const auto value = std::to_string(std::get<double>(immediate));
                _directive("\t" + name + ": .double\t" + value, _data);
                return Memory(Size::QWORD, name);
            }

            if (std::holds_alternative<float>(immediate)) {
                auto name = "float" + std::to_string(_constants++);
                const auto value = std::to_string(std::get<float>(immediate));
                _directive("\t" + name + ": .float\t" + value, _data);
                return Memory(Size::DWORD, name);
            }

            return std::visit([](const auto &value) -> Immediate { return value; }, immediate);
        },
        [&](const il::Memory &memory) -> Operand {
            return _current_mapper[memory];
        },
        [&](const il::Variable &variable) -> Operand {
            return _current_mapper[variable];
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
        const auto &instruction = (type.size() == Size::QWORD) ? &Generator::_movsd : &Generator::_movss;
        (this->*instruction)(destination, source);
    } else {
        _mov(destination, source);
    }
}

Register Generator::_store_temp_1(const Operand &source, const sem::Type &type) {
    auto temp = _temp_1_register(type);
    _store(source, temp, type);
    return temp;
}

Register Generator::_temp_1_register(const sem::Type &type) {
    auto reg_base = (std::holds_alternative<sem::Floating>(type) ? Register::Base::XMM10 : Register::Base::R10);
    return {reg_base, type.size()};
}

Register Generator::_store_temp_2(const Operand &source, const sem::Type &type) {
    auto temp = _temp_2_register(type);
    _store(source, temp, type);
    return temp;
}

Register Generator::_temp_2_register(const sem::Type &type) {
    auto reg_base = (std::holds_alternative<sem::Floating>(type) ? Register::Base::XMM11 : Register::Base::R11);
    return {reg_base, type.size()};
}

Register Generator::_adjust_to_reg(const Operand &, const Operand &target, const sem::Type &type) {
    // Early exit if the target operand already is a register.
    if (std::holds_alternative<Register>(target)) return std::get<Register>(target);

    // Otherwise if the result is not a register, store the lhs in a temp register.
    return _store_temp_1(target, type);
}

void Generator::_directive(const std::string &directive, std::vector<AssemblyItem> &output) {
    output.push_back(Directive(directive));
}

void Generator::_label(const std::string &name) {
    _text.push_back(Label(name));
}

void Generator::_jmp(const std::string &name) {
    _text.push_back(Instruction(Instruction::Opcode::JMP, {name}));
}

void Generator::_jnz(const std::string &name) {
    _text.push_back(Instruction(Instruction::Opcode::JNZ, {name}));
}

void Generator::_call(const std::string &name) {
    _text.push_back(Instruction(Instruction::Opcode::CALL, {name}));
}

void Generator::_movsxd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOVSXD, {destination, source}));
}

void Generator::_movsd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOVSD, {destination, source}));
}

void Generator::_movss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOVSS, {destination, source}));
}

void Generator::_movzx(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOVZX, {destination, source}));
}

void Generator::_movsx(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOVSX, {destination, source}));
}

void Generator::_mov(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MOV, {destination, source}));
}

void Generator::_addsd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::ADDSD, {destination, source}));
}

void Generator::_addss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::ADDSS, {destination, source}));
}

void Generator::_add(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::ADD, {destination, source}));
}

void Generator::_subsd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::SUBSD, {destination, source}));
}

void Generator::_subss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::SUBSS, {destination, source}));
}

void Generator::_sub(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::SUB, {destination, source}));
}

void Generator::_mulsd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MULSD, {destination, source}));
}

void Generator::_mulss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::MULSS, {destination, source}));
}

void Generator::_imul(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::IMUL, {destination, source}));
}

void Generator::_divsd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::DIVSD, {destination, source}));
}

void Generator::_divss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::DIVSS, {destination, source}));
}

void Generator::_idiv(const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::IDIV, {source}));
}

void Generator::_udiv(const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::DIV, {source}));
}

void Generator::_xorps(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::XORPS, {destination, source}));
}

void Generator::_or(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::OR, {destination, source}));
}

void Generator::_ucomisd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::UCOMISD, {destination, source}));
}

void Generator::_ucomiss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::UCOMISS, {destination, source}));
}

void Generator::_cvtsd2ss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTSD2SS, {destination, source}));
}

void Generator::_cvtss2sd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTSS2SD, {destination, source}));
}

void Generator::_cvtsi2sd(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTSI2SD, {destination, source}));
}

void Generator::_cvtsi2ss(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTSI2SS, {destination, source}));
}

void Generator::_cvttsd2si(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTTSD2SI, {destination, source}));
}

void Generator::_cvttss2si(const Operand &destination, const Operand &source) {
    _text.push_back(Instruction(Instruction::Opcode::CVTTSS2SI, {destination, source}));
}

void Generator::_test(const Operand &first, const Operand &second) {
    _text.push_back(Instruction(Instruction::Opcode::TEST, {first, second}));
}

void Generator::_cmp(const Operand &first, const Operand &second) {
    _text.push_back(Instruction(Instruction::Opcode::CMP, {first, second}));
}

void Generator::_setne(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETNE, {destination}));
}

void Generator::_setg(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETG, {destination}));
}

void Generator::_seta(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETA, {destination}));
}

void Generator::_setb(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETB, {destination}));
}

void Generator::_setl(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETL, {destination}));
}

void Generator::_setp(const Operand &destination) {
    _text.push_back(Instruction(Instruction::Opcode::SETP, {destination}));
}

void Generator::_enter(uint16_t size, uint8_t nesting_level) {
    _text.push_back(Instruction(Instruction::Opcode::ENTER, {size, nesting_level}));
}

void Generator::_syscall() {
    _text.push_back(Instruction(Instruction::Opcode::SYSCALL, {}));
}

void Generator::_leave() {
    _text.push_back(Instruction(Instruction::Opcode::LEAVE, {}));
}

void Generator::_ret() {
    _text.push_back(Instruction(Instruction::Opcode::RET, {}));
}

void Generator::_newline(std::vector<AssemblyItem> &output) {
    _directive("", output);
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
