template<typename Type, typename... Args>
SharedSymbol &SymbolTable::insert(const std::string &name, Args &&... args) {
    if (_symbols.contains(name)) throw IdentifierAlreadyTaken(name);

    auto symbol = std::make_shared<SymbolType>(Type(name, std::forward<Args>(args)...));
    auto result = _symbols.emplace(name, symbol);

    return result.first->second;
}

template<typename... Types>
[[nodiscard]] SharedSymbol &SymbolTable::lookup(const std::string &name) {
    auto found = _symbols.find(name);
    if (found != _symbols.end() && (std::holds_alternative<Types>(*found->second) || ...)) {
        return found->second;
    }

    if (_parent == nullptr) throw IdentifierNotFound(name);

    return _parent->lookup<Types...>(name);
}