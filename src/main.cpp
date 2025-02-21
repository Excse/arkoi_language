#include <iostream>
#include <fstream>
#include <sstream>

#include "opt/dead_code_elimination.hpp"
#include "opt/constant_propagation.hpp"
#include "opt/constant_folding.hpp"
#include "opt/simplify_cfg.hpp"
#include "opt/pass.hpp"
#include "sem/name_resolver.hpp"
#include "sem/type_resolver.hpp"
#include "x86_64/generator.hpp"
#include "il/cfg_printer.hpp"
#include "il/il_printer.hpp"
#include "il/generator.hpp"
#include "front/scanner.hpp"
#include "front/parser.hpp"

using namespace arkoi;

void dump_cfg(const std::string &base_path, il::Module &module) {
    auto cfg_output = il::CFGPrinter::print(module);

    auto dot_path = base_path + ".dot";
    auto png_path = base_path + ".png";

    std::ofstream cfg_file(dot_path);
    cfg_file << cfg_output.str();
    cfg_file.close();

    std::string assemble_command = "dot -Tpng " + dot_path + " -o " + png_path;
    int assemble_result = std::system(assemble_command.c_str());
    if (WEXITSTATUS(assemble_result) != 0) exit(1);
}

int main() {
    static const std::string INPUT_FILE = "../example/test.ark";

    auto last_dot = INPUT_FILE.find_last_of('.');
    if (last_dot == std::string::npos || INPUT_FILE.substr(last_dot) != ".ark") {
        throw std::invalid_argument("This is not a valid file path with '.ark' extension.");
    }

    auto base_path = INPUT_FILE.substr(0, last_dot);

    std::ifstream file(INPUT_FILE);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    std::cout << "~~~~~~~~~~~~         Lex & Scan           ~~~~~~~~~~~~ " << std::endl;

    front::Scanner scanner(source);
    front::Parser parser(scanner.tokenize());
    auto program = parser.parse_program();

    if (scanner.has_failed() || parser.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~        Name Resolver         ~~~~~~~~~~~~" << std::endl;

    auto name_resolver = sem::NameResolver::resolve(program);
    if (name_resolver.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~        Type Resolver         ~~~~~~~~~~~~" << std::endl;

    auto type_resolver = sem::TypeResolver::resolve(program);
    if (type_resolver.has_failed()) exit(1);

    std::cout << "~~~~~~~~~~~~    Intermediate Language     ~~~~~~~~~~~~" << std::endl;

    auto module = il::Generator::generate(program);

    auto il_org_output = il::ILPrinter::print(module);
    std::cout << il_org_output.str();

    dump_cfg(base_path + "_org", module);

    std::cout << "~~~~~~~~~~~~       Optimizing IL          ~~~~~~~~~~~~" << std::endl;

    opt::PassManager manager;
    manager.add<opt::ConstantFolding>();
    manager.add<opt::ConstantPropagation>();
    manager.add<opt::DeadCodeElimination>();
    manager.add<opt::SimplifyCFG>();
    manager.run(module);

    auto il_opt_output = il::ILPrinter::print(module);
    std::cout << il_opt_output.str();

    dump_cfg(base_path + "_opt", module);

    std::cout << "~~~~~~~~       Generating Assembly          ~~~~~~~~" << std::endl;

    auto assembly_output = x86_64::Generator::generate(module);
    std::cout << assembly_output.str();

    std::ofstream cfg_file(base_path + ".asm");
    cfg_file << assembly_output.str();
    cfg_file.close();

    return 0;
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================

