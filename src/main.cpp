#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "opt/constant_propagation.hpp"
#include "opt/constant_folding.hpp"
#include "opt/dce.hpp"
#include "back/x86_64/generator.hpp"
#include "mid/name_resolver.hpp"
#include "mid/type_resolver.hpp"
#include "mid/generator.hpp"
#include "mid/printer.hpp"
#include "front/scanner.hpp"
#include "front/parser.hpp"

using namespace arkoi;

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    std::cout << "~~~~~~~~~~~~         Lex & Scan           ~~~~~~~~~~~~ " << std::endl;

    front::Scanner scanner(source);
    front::Parser parser(scanner.tokenize());
    auto program = parser.parse_program();

    if (scanner.has_failed() || parser.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~        Name Resolver         ~~~~~~~~~~~~" << std::endl;

    auto name_resolver = mid::NameResolver::resolve(program);
    if (name_resolver.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~        Type Resolver         ~~~~~~~~~~~~" << std::endl;

    auto type_resolver = mid::TypeResolver::resolve(program);
    if (type_resolver.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~    Intermediate Language     ~~~~~~~~~~~~" << std::endl;

    auto il_generator = mid::Generator::generate(program);
    auto il_printer = mid::Printer::print(il_generator.functions());
    std::cout << il_printer.output().str();

    std::cout << "~~~~~~~~~~~~       Optimizing IL          ~~~~~~~~~~~~" << std::endl;

    opt::PassManager optimization_manager;
    optimization_manager.emplace<opt::ConstantFolding>();
    optimization_manager.emplace<opt::ConstantPropagation>();
    optimization_manager.emplace<opt::DeadCodeElimination>();
    optimization_manager.optimize(il_generator.functions());

    std::cout << "~~~~~~~~~~~~          Optimized           ~~~~~~~~~~~~" << std::endl;

    auto optimized_printer = mid::Printer::print(il_generator.functions());
    std::cout << optimized_printer.output().str();

    std::cout << "~~~~~~~~~~~~       GNU Assembler          ~~~~~~~~~~~~" << std::endl;

    auto gas_generator = back::x86_64::Generator::generate(il_generator.functions());
    std::cout << gas_generator.output().str();

    auto temp_dir = std::filesystem::temp_directory_path();
    auto asm_file_path = temp_dir / "temp_asm.s";
    auto obj_file_path = temp_dir / "temp_obj.o";
    auto exe_file_path = temp_dir / "temp_executable";

    std::ofstream asm_file(asm_file_path);
    asm_file << gas_generator.output().str();
    asm_file.close();

    std::cout << "~~~~~~~~~~~~          Assemble            ~~~~~~~~~~~~" << std::endl;

    std::string assemble_command = "as " + asm_file_path.string() + " -o " + obj_file_path.string();
    int assemble_result = std::system(assemble_command.c_str());
    if (WEXITSTATUS(assemble_result) != 0) exit(1);

    std::cout << "~~~~~~~~~~~~            Link              ~~~~~~~~~~~~" << std::endl;

    std::string link_command = "ld " + obj_file_path.string() + " -o " + exe_file_path.string();
    int link_result = std::system(link_command.c_str());
    if (WEXITSTATUS(link_result) != 0) exit(1);

    std::cout << "~~~~~~~~~~~~           Execute            ~~~~~~~~~~~~" << std::endl;

    int exec_result = std::system(exe_file_path.string().c_str());
    std::cout << "Execute Code: " << WEXITSTATUS(exec_result) << std::endl;

    return 0;
}
