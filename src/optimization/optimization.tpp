template<typename IterativeType, typename... Args>
IterativeType &OptimizationManager::emplace(Args &&... args) {
    _passes.push_back(std::make_unique<IterativeType>(std::forward<Args>(args)...));
    return reinterpret_cast<IterativeType &>(*_passes.back());
}