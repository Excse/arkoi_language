#include "symbol_table.h"

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
