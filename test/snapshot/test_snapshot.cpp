#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "frontend/scanner.hpp"
#include "snapshot.hpp"

TEST(Snapshot, Scanner) {
    static const std::string FILES = TEST_PATH "/snapshot/scanner/";

    for (const auto &entry: std::filesystem::directory_iterator(FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        auto file_name = entry.path().filename().stem().string();
        auto snapshot_file = FILES + file_name + ".snapshot";

        SnapshotTester tester(snapshot_file);

        std::ifstream source_file(entry.path().string());
        std::stringstream source;
        source << source_file.rdbuf();

        auto scanner = Scanner(source.str());

        std::stringstream output;
        for (const auto &token: scanner.tokenize()) {
            output << token << "\n";
        }

        ASSERT_TRUE(tester.compare(output.str()));
    }
}