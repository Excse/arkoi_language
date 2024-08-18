#include "symbol_table.h"

std::ostream &operator<<(std::ostream &os, const Symbol &symbol) {
    os << symbol.name();
    return os;
}

std::shared_ptr<Symbol> &SymbolTable::insert(const std::string &name, Symbol::Type type) {
    if (_symbols.find(name) != _symbols.end()) {
        throw IdentifierAlreadyTaken(name);
    }

    auto result = _symbols.emplace(name, std::make_shared<Symbol>(name, type));
    return result.first->second;
}

std::shared_ptr<Symbol> &SymbolTable::lookup(const std::string &name,
                                             const std::function<bool(const Symbol &)> &predicate) {
    auto found = _symbols.find(name);
    if (found != _symbols.end() && predicate(*found->second)) {
        return found->second;
    }

    if (_parent != nullptr) {
        return _parent->lookup(name, predicate);
    }

    throw IdentifierNotFound(name);
}

std::shared_ptr<Symbol> &SymbolTable::lookup_any(const std::string &name) {
    return lookup(name, [](const Symbol &) { return true; });
}
