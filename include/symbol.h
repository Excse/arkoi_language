#ifndef ARKOI_LANGUAGE_SYMBOL_H
#define ARKOI_LANGUAGE_SYMBOL_H

#include <iostream>
#include <variant>
#include <string>

class FunctionSymbol {
public:
    explicit FunctionSymbol(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] const auto &name() const { return _name; }

private:
    std::string _name;
};

class ParameterSymbol {
public:
    explicit ParameterSymbol(std::string name, size_t index) : _name(std::move(name)), _index(index) {}

    [[nodiscard]] const auto &name() const { return _name; }

    [[nodiscard]] auto index() const { return _index; }

private:
    std::string _name;
    size_t _index;
};

class TemporarySymbol {
public:
    explicit TemporarySymbol(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] const auto &name() const { return _name; }

private:
    std::string _name;
};

using Symbol = std::variant<FunctionSymbol, ParameterSymbol, TemporarySymbol>;

std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

#endif //ARKOI_LANGUAGE_SYMBOL_H