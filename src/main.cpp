#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

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
    auto program = parser.parse_program();

    if(scanner.has_failed() || parser.has_failed()) {
        exit(1);
    }

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

    auto temp_dir = std::filesystem::temp_directory_path();
    auto asm_file_path = temp_dir / "temp_asm.s";
    auto obj_file_path = temp_dir / "temp_obj.o";
    auto exe_file_path = temp_dir / "temp_executable";

    std::ofstream asm_file(asm_file_path);
    asm_file << gas_generator.output();
    asm_file.close();

    std::string assemble_command = "as " + asm_file_path.string() + " -o " + obj_file_path.string();
    int assemble_result = std::system(assemble_command.c_str());
    if (WEXITSTATUS(assemble_result) != 0) {
        std::cerr << "Assembly failed" << std::endl;
        exit(1);
    }

    std::string link_command = "ld " + obj_file_path.string() + " -o " + exe_file_path.string();
    int link_result = std::system(link_command.c_str());
    if (WEXITSTATUS(link_result) != 0) {
        std::cerr << "Linking failed" << std::endl;
        exit(1);
    }

    std::cout << "~~~~~~~~~~~~ Execute ~~~~~~~~~~~~ " << std::endl;

    int exec_result = std::system(exe_file_path.string().c_str());
    std::cout << "Execute Code: " << WEXITSTATUS(exec_result)  << std::endl;

    return 0;
}
