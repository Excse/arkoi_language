#include "il/operand.hpp"

#include "utils/utils.hpp"

namespace il {

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

}

namespace std {

size_t hash<il::Variable>::operator()(const il::Variable &variable) const {
    size_t linkHash = std::hash<std::shared_ptr<SymbolType>>{}(variable.symbol());
    size_t generationHash = std::hash<size_t>{}(variable.version());
    return linkHash ^ (generationHash << 1);
}


size_t hash<il::Constant>::operator()(const il::Constant &constant) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, constant);
}

size_t hash<il::Operand>::operator()(const il::Operand &operand) const {
    return std::visit([](const auto &value) -> size_t {
        return std::hash<std::decay_t<decltype(value)>>{}(value);
    }, operand);
}

}

std::ostream &operator<<(std::ostream &os, const il::Constant &constant) {
    std::visit(match{
        [&os](const bool &value) { os << (value ? "1" : "0"); },
        [&os](const auto &value) { os << value; },
    }, constant);
    return os;
}

std::ostream &operator<<(std::ostream &os, const il::Variable &variable) {
    os << variable.symbol();
    if(variable.version() != 0) os << variable.version();
    return os;
}

std::ostream &operator<<(std::ostream &os, const il::Operand &operand) {
    std::visit([&os](const auto &other) { os << other; }, operand);
    return os;
}