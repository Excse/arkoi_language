#include <fstream>
#include <iostream>
#include <sstream>

#include "scanner.h"

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();
    Scanner scanner(source);

    std::vector<Token> tokens = scanner.tokenize();
    for (auto &token: tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
