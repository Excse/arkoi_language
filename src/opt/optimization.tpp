template<typename Type, typename... Args>
Type &PassManager::emplace(Args &&... args) {
    _passes.push_back(std::make_unique<Type>(std::forward<Args>(args)...));
    return reinterpret_cast<Type &>(*_passes.back());
}