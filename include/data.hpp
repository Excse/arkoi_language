#pragma once

#include <iostream>

enum class Size : size_t {
    BYTE = 8,
    WORD = 16,
    DWORD = 32,
    QWORD = 64,
};

std::ostream &operator<<(std::ostream &os, const Size &size);

size_t _size_to_bytes(const Size &size);

size_t _size_to_bits(const Size &size);