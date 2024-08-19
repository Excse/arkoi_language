cmake_minimum_required(VERSION 3.28)

project(arkoi_language)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
add_executable(arkoi_language
        src/main.cpp
        src/scanner.cpp
        include/scanner.h
        src/token.cpp
        include/token.h
        src/parser.cpp
        include/parser.h
        include/ast.h
        include/visitor.h
        src/symbol_table.cpp
        include/symbol_table.h
        src/name_resolution.cpp
        include/name_resolution.h
        src/tac.cpp
        include/tac.h
        src/il_generator.cpp
        include/il_generator.h
        include/utils.h
        src/il_printer.cpp
        include/il_printer.h
        src/gas_generator.cpp
        include/gas_generator.h)