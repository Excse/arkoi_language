#ifndef ARKOI_LANGUAGE_UTILS_H
#define ARKOI_LANGUAGE_UTILS_H

#include <sstream>

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

#endif //ARKOI_LANGUAGE_UTILS_H
