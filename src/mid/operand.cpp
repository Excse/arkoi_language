#include "mid/operand.hpp"

#include "utils/utils.hpp"

using namespace arkoi::mid;

bool Variable::operator==(const Variable &rhs) const {
    return _symbol == rhs._symbol && _version == rhs._version;
}

bool Variable::operator!=(const Variable &rhs) const {
    return !(rhs == *this);
}

Size Constant::size() const {
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

std::ostream &operator<<(std::ostream &os, const Constant &constant) {
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, constant);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
    os << variable.symbol();
    if(variable.version() != 0) os << variable.version();
    return os;
}

namespace std {

size_t hash<arkoi::mid::Variable>::operator()(const arkoi::mid::Variable &variable) const {
    size_t linkHash = std::hash<std::shared_ptr<SymbolType>>{}(variable.symbol());
    size_t generationHash = std::hash<size_t>{}(variable.version());
    return linkHash ^ (generationHash << 1);
}


size_t hash<arkoi::mid::Constant>::operator()(const arkoi::mid::Constant &constant) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, constant);
}

size_t hash<arkoi::mid::Operand>::operator()(const arkoi::mid::Operand &operand) const {
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
