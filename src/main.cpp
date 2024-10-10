#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include "memory_resolver.hpp"
#include "name_resolver.hpp"
#include "type_resolver.hpp"
#include "gas_generator.hpp"
#include "il_generator.hpp"
#include "il_printer.hpp"
#include "scanner.hpp"
#include "parser.hpp"

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    std::cout << "~~~~~~~~~~~~         Lex & Scan           ~~~~~~~~~~~~ " << std::endl;

    Scanner scanner(source);
    Parser parser(scanner.tokenize());
    auto program = parser.parse_program();

    if (scanner.has_failed() || parser.has_failed()) {
        exit(1);
    }

    std::cout << "~~~~~~~~~~~~        Name Resolver         ~~~~~~~~~~~~" << std::endl;

    auto name_resolver = NameResolver::resolve(program);
    if (name_resolver.has_failed()) {
        exit(1);
    }

    std::cout << "~~~~~~~~~~~~        Type Resolver         ~~~~~~~~~~~~" << std::endl;

    auto type_resolver = TypeResolver::resolve(program);
    if (type_resolver.has_failed()) {
        exit(1);
    }

    std::cout << "~~~~~~~~~~~~    Intermediate Language     ~~~~~~~~~~~~" << std::endl;

    auto ir_generator = IRGenerator::generate(program);
    std::cout << ir_generator.functions().size() << std::endl;

//    auto printer = ILPrinter::print(ir_generator.instructions());
//    std::cout << printer.output().str();
//
//    std::cout << "~~~~~~~~~~~~       Memory Resolver        ~~~~~~~~~~~~" << std::endl;
//
//    auto memory_resolver = MemoryResolver::resolve(ir_generator.instructions());
//    for (const auto &item: memory_resolver.resolved()) {
//        std::cout << ":> " << *item.first << " = " << item.second << std::endl;
//    }
//
//    std::cout << "~~~~~~~~~~~~       GNU Assembler          ~~~~~~~~~~~~" << std::endl;
//
//    auto gas_generator = GASGenerator::generate(ir_generator.instructions(), memory_resolver.data());
//    std::cout << gas_generator.output().str();
//
//    auto temp_dir = std::filesystem::temp_directory_path();
//    auto asm_file_path = temp_dir / "temp_asm.s";
//    auto obj_file_path = temp_dir / "temp_obj.o";
//    auto exe_file_path = temp_dir / "temp_executable";
//
//    std::ofstream asm_file(asm_file_path);
//    asm_file << gas_generator.output().str();
//    asm_file.close();
//
//    std::cout << "~~~~~~~~~~~~          Assemble            ~~~~~~~~~~~~" << std::endl;
//
//    std::string assemble_command = "as " + asm_file_path.string() + " -o " + obj_file_path.string();
//    int assemble_result = std::system(assemble_command.c_str());
//    if (WEXITSTATUS(assemble_result) != 0) {
//        exit(1);
//    }
//
//    std::cout << "~~~~~~~~~~~~            Link              ~~~~~~~~~~~~" << std::endl;
//
//    std::string link_command = "ld " + obj_file_path.string() + " -o " + exe_file_path.string();
//    int link_result = std::system(link_command.c_str());
//    if (WEXITSTATUS(link_result) != 0) {
//        exit(1);
//    }
//
//    std::cout << "~~~~~~~~~~~~           Execute            ~~~~~~~~~~~~" << std::endl;
//
//    int exec_result = std::system(exe_file_path.string().c_str());
//    std::cout << "Execute Code: " << WEXITSTATUS(exec_result) << std::endl;

    return 0;
}
