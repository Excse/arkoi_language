#ifndef ARKOI_LANGUAGE_UTILS_H
#define ARKOI_LANGUAGE_UTILS_H

template<class... Ts>
struct match : Ts ... {
    using Ts::operator()...;
};

template<class... Ts>
match(Ts...) -> match<Ts...>;

#endif //ARKOI_LANGUAGE_UTILS_H
