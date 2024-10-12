template<typename OptimizationType, typename... Args>
OptimizationType &OptimizationManager::emplace_back(Args &&... args) {
    _passes.push_back(std::make_unique<OptimizationType>(std::forward<Args>(args)...));
    return reinterpret_cast<OptimizationType &>(*_passes.back());
}