template<typename InstructionType, typename... Args>
void arkoi::BasicBlock::emplace_back(Args &&... args) {
    _instructions.emplace_back(std::make_unique<InstructionType>(std::forward<Args>(args)...));
}