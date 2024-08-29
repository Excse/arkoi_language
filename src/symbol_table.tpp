template <typename SymbolType, typename... Args>
std::shared_ptr<Symbol>& SymbolTable::insert(const std::string& name, Args&&... args) {
    if (_symbols.find(name) != _symbols.end()) {
        throw IdentifierAlreadyTaken(name);
    }

    auto symbol = std::make_shared<Symbol>(SymbolType(name, std::forward<Args>(args)...));
    auto result = _symbols.emplace(name, symbol);

    return result.first->second;
}