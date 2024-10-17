#pragma once

#include <iostream>

enum class Size : size_t {
    BYTE = 1,
    WORD = 2,
    DWORD = 4,
    QWORD = 8,
};

std::ostream &operator<<(std::ostream &os, const Size &size);

size_t _size_to_bytes(const Size &size);

size_t _size_to_bits(const Size &size);