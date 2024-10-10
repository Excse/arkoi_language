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
# Google Test
# =====================

FetchContent_Declare(
        google_test
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(google_test)

# =====================
# Library
# =====================

include_directories(include)

add_library(${PROJECT_NAME}_lib
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
        src/gas_assembly.cpp
        src/data.cpp)

target_link_libraries(${PROJECT_NAME}_lib pretty_diagnostics)

# =====================
# Executable
# =====================

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# =====================
# Tests
# =====================

enable_testing()

add_executable(${PROJECT_NAME}_tests
        test/test_scanner.cpp)

target_link_libraries(${PROJECT_NAME}_tests gtest gtest_main ${PROJECT_NAME}_lib)

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})