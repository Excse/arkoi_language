template<typename SymbolType, typename... Args>
std::shared_ptr<Symbol> NameResolver::_check_non_existence(const arkoi::Token &token, Args &&... args) {
    try {
        return _scopes.top()->insert<SymbolType>(token.contents(), std::forward<Args>(args)...);
    } catch (const IdentifierAlreadyTaken &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
        return nullptr;
    }
}

template<typename... SymbolTypes>
std::shared_ptr<Symbol> NameResolver::_check_existence(const arkoi::Token &token) {
    try {
        return _scopes.top()->lookup<SymbolTypes...>(token.contents());
    } catch (const IdentifierNotFound &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
        return nullptr;
    }
}