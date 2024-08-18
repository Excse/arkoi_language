#include "symbol_table.h"

Symbol &SymbolTable::insert(const std::string &name, Symbol::Type type) {
    if (_symbols.find(name) != _symbols.end()) {
        throw IdentifierAlreadyTaken(name);
    }

    auto result = _symbols.emplace(name, Symbol(name, type));
    return result.first->second;
}

Symbol &SymbolTable::lookup(const std::string &name, const std::function<bool(const Symbol &)> &predicate) {
    auto found = _symbols.find(name);
    if (found != _symbols.end() && predicate(found->second)) {
        return found->second;
    }

    if (_parent != nullptr) {
        return _parent->lookup(name, predicate);
    }

    throw IdentifierNotFound(name);
}
