template<typename Type, typename... Args>
Symbol NameResolver::_check_non_existence(const Token &token, Args &&... args) {
    try {
        return _scopes.top()->insert<Type>(token.contents(), std::forward<Args>(args)...);
    } catch (const IdentifierAlreadyTaken &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
        return nullptr;
    }
}

template<typename... Types>
Symbol NameResolver::_check_existence(const Token &token) {
    try {
        return _scopes.top()->lookup<Types...>(token.contents());
    } catch (const IdentifierNotFound &error) {
        std::cout << error.what() << std::endl;
        _failed = true;
        return nullptr;
    }
}