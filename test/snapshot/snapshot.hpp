#pragma once

#include <string>

class SnapshotTester {
public:
    explicit SnapshotTester(std::string path) : _path(std::move(path)) {}

    void save(const std::string &data) const;

    std::string load() const;

    bool compare(const std::string &current) const;

private:
    std::string _path;
};