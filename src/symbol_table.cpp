//
// Created by timo on 8/14/24.
//

#include "symbol_table.h"

Symbol &SymbolTable::insert(const std::string &name) {
    if (_symbols.find(name) != _symbols.end()) {
        throw IdentifierAlreadyTaken(name);
    }

    auto result = _symbols.emplace(name, Symbol(name));
    return result.first->second;
}

Symbol &SymbolTable::lookup(const std::string &name) {
    auto found = _symbols.find(name);
    if (found == _symbols.end()) {
        throw IdentifierNotFound(name);
    }

    return found->second;
}
