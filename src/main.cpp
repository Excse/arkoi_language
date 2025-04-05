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
#include "il/analyses.hpp"
#include "front/scanner.hpp"
#include "front/parser.hpp"

using namespace arkoi;

void dump_cfg(const std::string &base_path, il::Module &module) {
    auto cfg_output = il::CFGPrinter::print(module);

    auto dot_path = base_path + ".dot";
    auto png_path = base_path + ".png";

    std::ofstream file(dot_path);
    file << cfg_output.str();

    std::string assemble_command = "dot -Tpng " + dot_path + " -o " + png_path;
    int assemble_result = std::system(assemble_command.c_str());
    if (WEXITSTATUS(assemble_result) != 0) exit(1);
}

int main(int argc, char* argv[]) {
    if(argc != 2) throw std::runtime_error("You need to provide a arkoi source path to compile.");
    const std::string input_path = argv[1];

    auto last_dot = input_path.find_last_of('.');
    if (last_dot == std::string::npos || input_path.substr(last_dot) != ".ark") {
        throw std::invalid_argument("This is not a valid file path with '.ark' extension.");
    }

    auto base_path = input_path.substr(0, last_dot);

    std::string source;
    {
        std::stringstream buffer;
        std::ifstream file(input_path);
        buffer << file.rdbuf();
        source = buffer.str();
    }

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

    {
        auto output = il::ILPrinter::print(module);
        std::ofstream out_file(base_path + "_org.il");
        out_file << output.str();
    }

    dump_cfg(base_path + "_org", module);

    std::cout << "~~~~~~~~~~~~       Optimizing IL          ~~~~~~~~~~~~" << std::endl;

    opt::PassManager manager;
    manager.add<opt::ConstantFolding>();
    manager.add<opt::ConstantPropagation>();
    manager.add<opt::DeadCodeElimination>();
    manager.add<opt::SimplifyCFG>();
    manager.run(module);

    {
        auto output = il::ILPrinter::print(module);
        std::ofstream out_file(base_path + "_opt.il");
        out_file << output.str();
    }

    dump_cfg(base_path + "_opt", module);

    std::cout << "~~~~~~~~       Generating Assembly          ~~~~~~~~" << std::endl;

    for (auto &function: module) {
        auto analysis = il::DataflowAnalysis<il::LivenessAnalysis>();
        analysis.run(function);

    }

    {
        auto output = x86_64::Generator::generate(module);
        std::ofstream out_file(base_path + ".asm");
        out_file << output.str();
    }

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

