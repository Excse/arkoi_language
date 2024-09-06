#ifndef ARKOI_LANGUAGE_SYMBOL_TABLE_H
#define ARKOI_LANGUAGE_SYMBOL_TABLE_H

#include <unordered_map>
#include <functional>
#include <optional>
#include <utility>
#include <memory>

#include "symbol.h"

class SymbolTable {
public:
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _parent(std::move(parent)) {}

    template<typename SymbolType, typename... Args>
    std::shared_ptr<Symbol> &insert(const std::string &name, Args &&... args);

    template<typename... SymbolTypes>
    [[nodiscard]] std::shared_ptr<Symbol> &lookup(const std::string &name);

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols{};
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

#include "../src/symbol_table.tpp"

#endif //ARKOI_LANGUAGE_SYMBOL_TABLE_H
