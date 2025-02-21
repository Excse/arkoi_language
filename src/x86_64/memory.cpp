#include "x86_64/memory.hpp"

#include "utils/utils.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

bool Memory::operator==(const Memory &other) const {
    return _index == other._index && _scale == other._scale && _displacement == other._displacement &&
           _address == other._address && _size == other._size;
}

bool Memory::operator!=(const Memory &other) const {
    return !(other == *this);
}

std::ostream &operator<<(std::ostream &os, const Memory::Address &memory) {
    std::visit([&os](const auto &value) { os << value; }, memory);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Memory &memory) {
    os << memory.size() << " PTR ";

    os << "[" << memory.address();

    if (memory.index() != 1) {
        os << " + " << memory.index();
    }

    if (memory.scale() != 1) {
        os << " * " << memory.scale();
    }

    if (memory.displacement() < 0) {
        os << " - " << std::abs(memory.displacement());
    } else if (memory.displacement() > 0) {
        os << " + " << std::abs(memory.displacement());
    }

    os << "]";
    return os;
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
