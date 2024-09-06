#ifndef ARKOI_LANGUAGE_SYMBOL_H
#define ARKOI_LANGUAGE_SYMBOL_H

#include <utility>
#include <vector>
#include <memory>

#include "type.h"

class Symbol {
public:
    explicit Symbol(std::string name) : _name(std::move(name)) {}

    virtual ~Symbol() = default;

    [[nodiscard]] auto &name() const { return _name; }

    friend std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

private:
    std::string _name;
};

class FunctionSymbol : public Symbol {
public:
    explicit FunctionSymbol(std::string name) : Symbol(std::move(name)) {}

    void set_parameter_types(std::vector<std::shared_ptr<Type>> &&types) { _parameter_types = std::move(types); }

    [[nodiscard]] auto &parameter_types() const { return _parameter_types; }

private:
    std::vector<std::shared_ptr<Type>> _parameter_types{};
};

class ParameterSymbol : public Symbol {
public:
    explicit ParameterSymbol(std::string name, size_t index) : Symbol(std::move(name)), _index(index) {}

    [[nodiscard]] auto index() const { return _index; }

    void set_type(std::shared_ptr<Type> type) { _type = std::move(type); }

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::shared_ptr<Type> _type{};
    size_t _index;
};

class TemporarySymbol : public Symbol {
public:
    explicit TemporarySymbol(std::string name, std::shared_ptr<Type> type)
            : Symbol(std::move(name)), _type(std::move(type)) {}

    [[nodiscard]] auto &type() const { return _type; }

private:
    std::shared_ptr<Type> _type{};
};

#endif //ARKOI_LANGUAGE_SYMBOL_H
