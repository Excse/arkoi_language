//
// Created by timo on 8/14/24.
//

#ifndef ARKOI_LANGUAGE_SYMBOL_TABLE_H
#define ARKOI_LANGUAGE_SYMBOL_TABLE_H

#include <unordered_map>
#include <optional>
#include <utility>
#include <memory>

class Symbol {
public:
    explicit Symbol(std::string name) : _name(std::move(name)) {}

    [[nodiscard]] const std::string &name() { return _name; }

private:
    std::string _name;
};

class SymbolTable {
public:
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _symbols(), _parent(std::move(parent)) {}

    Symbol &insert(const std::string &name);

    [[nodiscard]] Symbol &lookup(const std::string &name);

    [[nodiscard]] const std::shared_ptr<SymbolTable> &parent() { return _parent; }

private:
    std::unordered_map<std::string, Symbol> _symbols;
    std::shared_ptr<SymbolTable> _parent;
};

class IdentifierAlreadyTaken : public std::runtime_error {
public:
    explicit IdentifierAlreadyTaken(const std::string &name) : std::runtime_error(
            "The identifier " + name + " is already taken.") {}
};

class IdentifierNotFound : public std::runtime_error {
public:
    explicit IdentifierNotFound(const std::string &name) : std::runtime_error(
            "The identifier " + name + " was not found.") {}
};

#endif //ARKOI_LANGUAGE_SYMBOL_TABLE_H
