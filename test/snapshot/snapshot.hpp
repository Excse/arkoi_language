#pragma once

#include <string>

class SnapshotTester {
public:
    SnapshotTester(std::string path) : _path(std::move(path)) {}

    void save(const std::string &data);

    std::string load();

    bool compare(const std::string &current);

private:
    std::string _path;
};