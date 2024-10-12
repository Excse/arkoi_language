template<typename InstructionType, typename... Args>
void BasicBlock::emplace_back(Args &&... args) {
    _instructions.push_back(InstructionType(args...));
}