template<typename SingleType, typename... Args>
SingleType &OptimizationManager::emplace_single(Args &&... args) {
    _single_passes.push_back(std::make_unique<SingleType>(std::forward<Args>(args)...));
    return reinterpret_cast<SingleType &>(*_single_passes.back());
}

template<typename IterativeType, typename... Args>
IterativeType &OptimizationManager::emplace_iterative(Args &&... args) {
    _iterative_passes.push_back(std::make_unique<IterativeType>(std::forward<Args>(args)...));
    return reinterpret_cast<IterativeType &>(*_iterative_passes.back());
}