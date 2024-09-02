template<typename... SymbolTypes>
[[nodiscard]] std::shared_ptr<Symbol> &SymbolTable::lookup(const std::string &name) {
    auto found = _symbols.find(name);
    if (found != _symbols.end() && (std::dynamic_pointer_cast<SymbolTypes>(found->second) || ...)) {
        return found->second;
    }

    if (_parent != nullptr) {
        return _parent->lookup<SymbolTypes...>(name);
    }

    throw IdentifierNotFound(name);
}

template<typename SymbolType, typename... Args>
std::shared_ptr<Symbol> &SymbolTable::insert(const std::string &name, Args &&... args) {
    if (_symbols.find(name) != _symbols.end()) {
        throw IdentifierAlreadyTaken(name);
    }

    auto symbol = std::make_shared<SymbolType>(name, std::forward<Args>(args)...);
    auto result = _symbols.emplace(name, symbol);

    return result.first->second;
}