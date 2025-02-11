#pragma once

#include <cstddef>
#include <utility>

#include "mid/operand.hpp"
#include "def/size.hpp"

namespace arkoi::back {

class OperandBase {
public:
    virtual ~OperandBase() = default;
};

class RegisterBase : public OperandBase {
public:
    using RegisterID = std::size_t;

public:
    RegisterBase(RegisterID id, Size size) : _reg_id(id), _size(size) {}

    bool operator==(const RegisterBase &other) const;

    bool operator!=(const RegisterBase &other) const;

    [[nodiscard]] auto reg_id() const { return _reg_id; }

    [[nodiscard]] auto size() const { return _size; }

private:
    RegisterID _reg_id;
    Size _size;
};

class MemoryBase : public OperandBase {
public:
    using LabelAddress = std::string;
    using DirectAddress = int64_t;

    struct Address : std::variant<LabelAddress, DirectAddress, RegisterBase> {
        using variant::variant;
    };

public:
    MemoryBase(Size size, Address address, int64_t index = 1, int64_t scale = 1, int64_t displacement = 1)
        : _index(index), _scale(scale), _displacement(displacement), _address(std::move(address)), _size(size) {}

    bool operator==(const MemoryBase &other) const;

    bool operator!=(const MemoryBase &other) const;

    [[nodiscard]] auto displacement() const { return _displacement; }

    [[nodiscard]] auto &address() const { return _address; }

    [[nodiscard]] auto scale() const { return _scale; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto &size() const { return _size; }

private:
    int64_t _index, _scale, _displacement;
    Address _address;
    Size _size;
};

class ConstantBase : public OperandBase, public mid::Constant {
public:
    using mid::Constant::Constant;

    ConstantBase(mid::Constant constant);
};

} // namespace arkoi::back

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
