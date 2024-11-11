template<typename Type, typename... Args>
il::InstructionType &BasicBlock::emplace(Args &&... args) {
    return _instructions.emplace_back(Type(std::forward<Args>(args)...));
}