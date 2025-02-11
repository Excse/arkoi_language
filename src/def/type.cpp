#include "def/type.hpp"

#include <utility>

using namespace arkoi::type;

bool Integral::operator==(const Integral &other) const {
    return _size == other._size && _sign == other._sign;
}

bool Integral::operator!=(const Integral &other) const {
    return !(other == *this);
}

uint64_t Integral::max() const {
    switch (_size) {
        case Size::BYTE: return _sign ? std::numeric_limits<int8_t>::max() : std::numeric_limits<uint8_t>::max();
        case Size::WORD: return _sign ? std::numeric_limits<int16_t>::max() : std::numeric_limits<uint16_t>::max();
        case Size::DWORD: return _sign ? std::numeric_limits<int32_t>::max() : std::numeric_limits<uint32_t>::max();
        case Size::QWORD: return _sign ? std::numeric_limits<int64_t>::max() : std::numeric_limits<uint64_t>::max();
    }

    // As the -Wswitch flag is set, this will never be reached.
    std::unreachable();
}

bool Floating::operator==(const Floating &other) const {
    return _size == other._size;
}

bool Floating::operator!=(const Floating &other) const {
    return !(other == *this);
}

bool Boolean::operator==(const Boolean &) const {
    return true;
}

bool Boolean::operator!=(const Boolean &) const {
    return false;
}

Size Type::size() const {
    return std::visit([](const auto &value) { return value.size(); }, *this);
}

std::ostream &operator<<(std::ostream &os, const Type &type) {
    std::visit([&os](const auto &value) { os << value; }, type);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Integral &type) {
    return os << (type.sign() ? "s" : "u") << size_to_bits(type.size());
}

std::ostream &operator<<(std::ostream &os, const Floating &type) {
    return os << "f" << size_to_bits(type.size());
}

std::ostream &operator<<(std::ostream &os, const Boolean &) {
    return os << "bool";
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
