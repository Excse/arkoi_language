template<typename... SymbolTypes>
[[nodiscard]] std::shared_ptr<Symbol> &SymbolTable::lookup(const std::string &name) {
    auto found = _symbols.find(name);
    if (found != _symbols.end() && (std::holds_alternative<SymbolTypes>(*found->second) || ...)) {
        return found->second;
    }

    if (_parent == nullptr) throw IdentifierNotFound(name);

    return _parent->lookup<SymbolTypes...>(name);
}

template<typename SymbolType, typename... Args>
std::shared_ptr<Symbol> &SymbolTable::insert(const std::string &name, Args &&... args) {
    if (_symbols.contains(name)) throw IdentifierAlreadyTaken(name);

    auto symbol = std::make_shared<Symbol>(SymbolType(name, std::forward<Args>(args)...));
    auto result = _symbols.emplace(name, symbol);

    return result.first->second;
}