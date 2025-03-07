#include "il/operand.hpp"

#include <iomanip>

#include "utils/utils.hpp"

using namespace arkoi::il;

bool Memory::operator<(const Memory &rhs) const {
    return _name < rhs._name;
}

bool Memory::operator==(const Memory &rhs) const {
    return _name == rhs._name;
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

Size Immediate::size() const {
    return std::visit(match{
        [](const double &) { return Size::QWORD; },
        [](const float &) { return Size::DWORD; },
        [](const bool &) { return Size::BYTE; },
        [](const uint32_t &) { return Size::DWORD; },
        [](const int32_t &) { return Size::DWORD; },
        [](const uint64_t &) { return Size::QWORD; },
        [](const int64_t &) { return Size::QWORD; },
    }, *this);
}

std::ostream &operator<<(std::ostream &os, const Immediate &immediate) {
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, immediate);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
    os << variable.name();

    if (variable.version() != 0) {
        os << std::setw(2) << std::setfill('0') << variable.version();
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Memory &memory) {
    os << memory.name();
    return os;
}

Size Operand::size() const {
    return std::visit([](const auto &value) { return value.size(); }, *this);
}

namespace std {

size_t hash<arkoi::il::Variable>::operator()(const arkoi::il::Variable &variable) const {
    size_t name_hash = std::hash<std::string>{}(variable.name());
    size_t generation_hash = std::hash<size_t>{}(variable.version());
    return name_hash ^ (generation_hash << 1);
}

size_t hash<arkoi::il::Memory>::operator()(const arkoi::il::Memory &memory) const {
    return std::hash<std::string>{}(memory.name());
}

size_t hash<arkoi::il::Immediate>::operator()(const arkoi::il::Immediate &immediate) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, immediate);
}

size_t hash<arkoi::il::Operand>::operator()(const arkoi::il::Operand &operand) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, operand);
}

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
