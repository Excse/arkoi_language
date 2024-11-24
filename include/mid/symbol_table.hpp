#pragma once

#include <unordered_map>
#include <utility>
#include <memory>

#include "def/symbol.hpp"

namespace arkoi::mid {

class SymbolTable {
public:
    SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _parent(std::move(parent)) {}

    template<typename Type, typename... Args>
    SharedSymbol &insert(const std::string &name, Args &&... args);

    template<typename... Types>
    [[nodiscard]] SharedSymbol &lookup(const std::string &name);

private:
    std::unordered_map<std::string, SharedSymbol> _symbols{};
    std::shared_ptr<SymbolTable> _parent;
};

class IdentifierAlreadyTaken : public std::runtime_error {
public:
    IdentifierAlreadyTaken(const std::string &name)
        : std::runtime_error("The identifier " + name + " is already taken.") {}
};

class IdentifierNotFound : public std::runtime_error {
public:
    IdentifierNotFound(const std::string &name)
        : std::runtime_error("The identifier " + name + " was not found.") {}
};

#include "../../src/mid/symbol_table.tpp"

} // namespace arkoi::mid