#include "cursor.h"

bool Cursor::_is_eof() {
    return _position >= _data.size();
}

Location Cursor::_current_location() {
    return Location{_column, _row};
}

void Cursor::_mark_start() {
    _start = _position;
}

std::string_view Cursor::_view() {
    return _data.substr(_start, (_position - _start));
}

void Cursor::_next() {
    if (_current_char() == '\n') {
        _column++;
        _row = 0;
    } else {
        _row++;
    }

    _position += 1;
}

void Cursor::_consume(char expected) {
    _consume([&](char input) { return input == expected; }, "Expected " + std::string(1, expected));
}

bool Cursor::_try_consume(char expected) {
    try {
        _consume(expected);
        return true;
    } catch (...) {
        return false;
    }
}

void Cursor::_consume(const std::function<bool(char)> &predicate, const std::string &error) {
    char current = _current_char();
    if (_position >= _data.size()) {
        throw UnexpectedEndOfFile();
    }

    if (!predicate(current)) {
        throw UnexpectedChar(error);
    }

    _next();
}

bool Cursor::_try_consume(const std::function<bool(char)> &predicate) {
    try {
        _consume(predicate, "");
        return true;
    } catch (...) {
        return false;
    }
}
