#include "snapshot.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

void SnapshotTester::save(const std::string &data) const {
    std::ofstream file(_path);
    file << data;
}

std::string SnapshotTester::load() const {
    std::ifstream file(_path);
    std::stringstream buffer;

    if (file) buffer << file.rdbuf();

    return buffer.str();
}

bool SnapshotTester::compare(const std::string &current) const {
    const std::string expected = load();

    if (expected.empty()) {
        save(current);
        throw std::invalid_argument("No snapshout found, saving current output as the snapshot.");
    }

    return expected == current;
}
