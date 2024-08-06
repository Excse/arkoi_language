#ifndef ARKOI_LANGUAGE_CURSOR_H
#define ARKOI_LANGUAGE_CURSOR_H

#include <string_view>
#include <functional>
#include <stdexcept>

class UnexpectedEndOfFile : public std::runtime_error {
public:
    UnexpectedEndOfFile() : std::runtime_error("Unexpectedly reached the End Of File") {}
};

class UnexpectedChar : public std::runtime_error {
public:
    explicit UnexpectedChar(const std::string &error) : std::runtime_error(error) {}
};

class Cursor {
public:
    explicit Cursor(std::string_view data) : _position(0), _start(0), _data(data) {}

protected:
    [[nodiscard]] char _current() { return _data[_position]; }

    [[nodiscard]] bool _is_eof();

    void _mark_start();

    [[nodiscard]] std::string_view _view();

    void _next();

    void _consume(char expected);

    void _consume(const std::function<bool(char)> &predicate, const std::string &error);

    bool _try_consume(char expected);

    bool _try_consume(const std::function<bool(char)> &predicate);

private:
    size_t _position, _start;
    std::string_view _data;
};


#endif //ARKOI_LANGUAGE_CURSOR_H
