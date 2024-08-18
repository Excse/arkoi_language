#include <fstream>
#include <iostream>
#include <sstream>

#include "name_resolution.h"
#include "symbol_table.h"
#include "scanner.h"
#include "parser.h"

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    Scanner scanner(source);
    Parser parser(scanner.tokenize());

    Program program = parser.parse_program();

    NameResolution resolution;
    program.accept(resolution);
    if(resolution.has_failed()) {
        exit(1);
    }

    return 0;
}
