#pragma once

#include <utility>
#include <vector>
#include <memory>

#include "type.hpp"

struct SymbolType;

using SharedSymbol = std::shared_ptr<SymbolType>;

namespace symbol {

class Function {
public:
    Function(std::string name) : _name(std::move(name)) {}

    void set_parameters(std::vector<SharedSymbol> &&symbols) { _parameter_symbols = std::move(symbols); }

    [[nodiscard]] auto &parameter_symbols() const { return _parameter_symbols; }

    void set_return_type(Type type) { _return_type = type; }

    [[nodiscard]] auto &return_type() const { return _return_type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::vector<SharedSymbol> _parameter_symbols{};
    std::optional<Type> _return_type{};
    std::string _name;
};

class Parameter {
public:
    Parameter(std::string name) : _name(std::move(name)) {}

    void set_type(Type type) { _type = type; }

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::optional<Type> _type{};
    std::string _name;
};

class Temporary {
public:
    Temporary(std::string name, Type type) : _type(type), _name(std::move(name)) {}

    Temporary(std::string name) : _type(), _name(std::move(name)) {}

    [[nodiscard]] auto &type() const { return _type; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::optional<Type> _type;
    std::string _name;
};

}

struct SymbolType : std::variant<symbol::Function, symbol::Parameter, symbol::Temporary> {
    using variant::variant;
};

std::ostream &operator<<(std::ostream &os, const SharedSymbol &symbol);