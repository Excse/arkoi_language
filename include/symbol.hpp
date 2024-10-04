#pragma once

#include <utility>
#include <vector>
#include <memory>

#include "type.hpp"

struct Symbol;

class FunctionSymbol {
public:
    explicit FunctionSymbol(std::string name) : _name(std::move(name)) {}

    void set_parameters(std::vector<std::shared_ptr<Symbol>> &&symbols) { _parameter_symbols = std::move(symbols); }

    [[nodiscard]] auto &parameter_symbols() const { return _parameter_symbols; }

    void set_return_type(Type type) { _return_type = type; }

    [[nodiscard]] auto &return_type() const { return _return_type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::vector<std::shared_ptr<Symbol>> _parameter_symbols{};
    Type _return_type{};
    std::string _name;
};

class ParameterSymbol {
public:
    explicit ParameterSymbol(std::string name) : _name(std::move(name)) {}

    void set_sse_index(size_t index) { _sse_index = index; }

    [[nodiscard]] auto sse_index() const { return _sse_index; }

    void set_int_index(size_t index) { _int_index = index; }

    [[nodiscard]] auto int_index() const { return _int_index; }

    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    size_t _int_index{}, _sse_index{};
    std::string _name;
    Type _type{};
};

class TemporarySymbol {
public:
    TemporarySymbol(std::string name, Type type) : _name(std::move(name)), _type(type) {}

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::string _name;
    Type _type;
};

struct Symbol : std::variant<FunctionSymbol, ParameterSymbol, TemporarySymbol> {
    using variant::variant;

    friend std::ostream &operator<<(std::ostream &os, const Symbol &symbol);
};