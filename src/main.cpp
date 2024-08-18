#include <fstream>
#include <iostream>
#include <sstream>

#include "name_resolution.h"
#include "gas_generator.h"
#include "il_generator.h"
#include "il_printer.h"
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
    if (resolution.has_failed()) {
        exit(1);
    }

    IRGenerator ir_generator;
    program.accept(ir_generator);

    std::cout << "~~~~~~~~~~~~ Intermediate Language ~~~~~~~~~~~~ " << std::endl;

    ILPrinter il_printer;
    for (const auto &item: ir_generator.instructions()) {
        item->accept(il_printer);
    }

    std::cout << "~~~~~~~~~~~~ GNU Assembler ~~~~~~~~~~~~ " << std::endl;

    GASGenerator gas_generator;
    for (const auto &item: ir_generator.instructions()) {
        item->accept(gas_generator);
    }

    std::cout << gas_generator.output() << std::endl;

    return 0;
}
