#pragma once

#include <unordered_map>
#include <functional>
#include <utility>
#include <memory>

#include "semantic/symbol.hpp"

class SymbolTable {
public:
    SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) : _parent(std::move(parent)) {}

    template<typename SymbolType, typename... Args>
    Symbol &insert(const std::string &name, Args &&... args);

    template<typename... SymbolTypes>
    [[nodiscard]] Symbol &lookup(const std::string &name);

private:
    std::unordered_map<std::string, Symbol> _symbols{};
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

#include "../../src/semantic/symbol_table.tpp"
