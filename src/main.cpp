#include <fstream>
#include <iostream>
#include <sstream>

#include "name_resolution.h"
#include "il_generator.h"
#include "scanner.h"
#include "parser.h"

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    Scanner scanner(source);
    Parser parser(scanner.tokenize());

    ProgramNode program = parser.parse_program();

    NameResolution resolution;
    program.accept(resolution);
    if(resolution.has_failed()) {
        exit(1);
    }

    IRGenerator generator;
    program.accept(generator);

    auto instructions = generator.instructions();
    for (const auto &item: instructions) {
        std::visit([&](const auto &item) { std::cout << item << std::endl; }, item);
    }


    return 0;
}
