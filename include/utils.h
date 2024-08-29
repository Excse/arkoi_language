#ifndef ARKOI_LANGUAGE_UTILS_H
#define ARKOI_LANGUAGE_UTILS_H

#include <sstream>

template<class... Ts>
struct match : Ts ... {
    using Ts::operator()...;
};

template<class... Ts>
match(Ts...) -> match<Ts...>;

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

#endif //ARKOI_LANGUAGE_UTILS_H
