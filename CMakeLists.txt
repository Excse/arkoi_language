cmake_minimum_required(VERSION 3.29.6)

project(arkoi_language VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# =====================
# Pretty Diagnostics
# =====================

FetchContent_Declare(
        pretty_diagnostics
        GIT_REPOSITORY https://github.com/Excse/pretty_diagnostics.git
        GIT_TAG        main
)

FetchContent_MakeAvailable(pretty_diagnostics)

# =====================
# Own project
# =====================

include_directories(include)

add_executable(arkoi_language
        src/main.cpp
        src/scanner.cpp
        src/token.cpp
        src/parser.cpp
        src/symbol_table.tpp
        src/name_resolver.tpp
        src/name_resolver.cpp
        src/instruction.cpp
        src/il_generator.cpp
        src/il_printer.cpp
        src/gas_generator.cpp
        src/memory_resolver.cpp
        src/operand.cpp
        src/symbol.cpp
        src/type_resolver.cpp
        src/type.cpp
        src/gas_assembly.cpp)

target_link_libraries(arkoi_language pretty_diagnostics)