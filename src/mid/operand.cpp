#include "mid/operand.hpp"

#include "utils/utils.hpp"

using namespace arkoi::mid;

bool Variable::operator==(const Variable &rhs) const {
    return _symbol == rhs._symbol && _version == rhs._version;
}

bool Variable::operator!=(const Variable &rhs) const {
    return !(rhs == *this);
}

Size Constant::size() const {
    return std::visit(match{
        [](const double &) { return Size::QWORD; },
        [](const float &) { return Size::DWORD; },
        [](const bool &) { return Size::BYTE; },
        [](const uint32_t &) { return Size::DWORD; },
        [](const int32_t &) { return Size::DWORD; },
        [](const uint64_t &) { return Size::QWORD; },
        [](const int64_t &) { return Size::QWORD; },
    }, *this);
}

std::ostream &operator<<(std::ostream &os, const Constant &constant) {
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, constant);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
    os << variable.symbol();
    if(variable.version() != 0) os << variable.version();
    return os;
}

namespace std {

size_t hash<arkoi::mid::Variable>::operator()(const arkoi::mid::Variable &variable) const {
    size_t linkHash = std::hash<std::shared_ptr<SymbolType>>{}(variable.symbol());
    size_t generationHash = std::hash<size_t>{}(variable.version());
    return linkHash ^ (generationHash << 1);
}


size_t hash<arkoi::mid::Constant>::operator()(const arkoi::mid::Constant &constant) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, constant);
}

size_t hash<arkoi::mid::Operand>::operator()(const arkoi::mid::Operand &operand) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, operand);
}

}