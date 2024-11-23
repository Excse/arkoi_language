template<typename Type, typename... Args>
mid::InstructionType &BasicBlock::emplace(Args &&... args) {
    return _instructions.emplace_back(Type(std::forward<Args>(args)...));
}