template<typename SymbolType, typename... Args>
void NameResolver::_check_non_existence(const Token &token, Args &&... args) {
    try {
        auto scope = _scopes.top();

        auto name = std::string(token.value());
        scope->insert<SymbolType>(name, std::forward<Args>(args)...);
    } catch (const IdentifierAlreadyTaken &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}

template<typename... SymbolTypes>
void NameResolver::_check_existence(const Token &token) {
    try {
        auto scope = _scopes.top();
        std::ignore = scope->lookup<SymbolTypes...>(std::string(token.value()));
    } catch (const IdentifierNotFound &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
    }
}