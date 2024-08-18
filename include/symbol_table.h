#ifndef ARKOI_LANGUAGE_SYMBOL_TABLE_H
#define ARKOI_LANGUAGE_SYMBOL_TABLE_H

#include <unordered_map>
#include <functional>
#include <optional>
#include <utility>
#include <memory>

class Symbol {
public:
    enum class Type {
        Function,
        Parameter,
    };

public:
    explicit Symbol(std::string name, Type type) : _name(std::move(name)), _type(type) {}

    [[nodiscard]] const std::string &name() const { return _name; }

    [[nodiscard]] const Type &type() const { return _type; }

private:
    std::string _name;
    Type _type;
};

std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

class SymbolTable {
public:
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _symbols(), _parent(std::move(parent)) {}

    std::shared_ptr<Symbol> &insert(const std::string &name, Symbol::Type type);

    [[nodiscard]] std::shared_ptr<Symbol> &lookup(const std::string &name,
                                                  const std::function<bool(const Symbol &)> &predicate);

    [[nodiscard]] std::shared_ptr<Symbol> &lookup_any(const std::string &name);

    [[nodiscard]] const std::shared_ptr<SymbolTable> &parent() const { return _parent; }

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;
    std::shared_ptr<SymbolTable> _parent;
};

class IdentifierAlreadyTaken : public std::runtime_error {
public:
    explicit IdentifierAlreadyTaken(const std::string &name)
            : std::runtime_error("The identifier " + name + " is already taken.") {}
};

class IdentifierNotFound : public std::runtime_error {
public:
    explicit IdentifierNotFound(const std::string &name)
            : std::runtime_error("The identifier " + name + " was not found.") {}
};

#endif //ARKOI_LANGUAGE_SYMBOL_TABLE_H
