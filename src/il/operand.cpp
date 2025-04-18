#include "il/operand.hpp"

#include <iomanip>

#include "utils/utils.hpp"

using namespace arkoi::il;
using namespace arkoi;

bool Memory::operator<(const Memory &rhs) const {
    return _index < rhs._index;
}

bool Memory::operator==(const Memory &rhs) const {
    return _index == rhs._index;
}

bool Memory::operator!=(const Memory &rhs) const {
    return !(rhs == *this);
}

bool Variable::operator<(const Variable &rhs) const {
    if (_name != rhs._name) return _name < rhs._name;
    return _version < rhs._version;
}

bool Variable::operator==(const Variable &rhs) const {
    return _name == rhs._name && _version == rhs._version;
}

bool Variable::operator!=(const Variable &rhs) const {
    return !(rhs == *this);
}

sem::Type Immediate::type() const {
    return std::visit(match{
        [](const double &) -> sem::Type { return sem::Floating(Size::QWORD); },
        [](const float &) -> sem::Type { return sem::Floating(Size::DWORD); },
        [](const bool &) -> sem::Type { return sem::Boolean(); },
        [](const uint32_t &) -> sem::Type { return sem::Integral(Size::DWORD, false); },
        [](const int32_t &) -> sem::Type { return sem::Integral(Size::DWORD, true); },
        [](const uint64_t &) -> sem::Type { return sem::Integral(Size::QWORD, false); },
        [](const int64_t &) -> sem::Type { return sem::Integral(Size::QWORD, true); },
    }, *this);
}

std::ostream &operator<<(std::ostream &os, const Immediate &operand) {
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, operand);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &operand) {
    os << operand.name();

    if (operand.version() != 0) {
        os << std::setw(2) << std::setfill('0') << operand.version();
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Memory &operand) {
    os << "%" << std::setw(2) << std::setfill('0') << operand.index();
    return os;
}

sem::Type Operand::type() const {
    return std::visit([](const auto &value) { return value.type(); }, *this);
}

namespace std {

size_t hash<Variable>::operator()(const Variable &variable) const noexcept {
    const size_t name_hash = std::hash<std::string>{}(variable.name());
    const size_t generation_hash = std::hash<size_t>{}(variable.version());
    return name_hash ^ (generation_hash << 1);
}

size_t hash<Memory>::operator()(const Memory &memory) const noexcept {
    return std::hash<size_t>{}(memory.index());
}

size_t hash<Immediate>::operator()(const Immediate &immediate) const noexcept {
    return std::visit([]<typename T>(const T &value) -> size_t {
        return std::hash<std::decay_t<T> >{}(value);
    }, immediate);
}

size_t hash<Operand>::operator()(const Operand &operand) const noexcept {
    return std::visit([]<typename T>(const T &value) -> size_t {
        return std::hash<std::decay_t<T> >{}(value);
    }, operand);
}

} // namespace std

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
