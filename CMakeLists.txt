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
        src/frontend/scanner.cpp
        src/frontend/token.cpp
        src/frontend/parser.cpp
        src/semantic/symbol_table.tpp
        src/semantic/name_resolver.tpp
        src/semantic/name_resolver.cpp
        src/semantic/type_resolver.cpp
        src/semantic/symbol.cpp
        src/semantic/type.cpp
        src/il/instruction.cpp
        src/il/generator.cpp
        src/il/printer.cpp
        src/il/operand.cpp
        src/il/cfg.cpp
        src/optimization/constant_propagation.cpp
        src/optimization/constant_folding.cpp
        src/optimization/dce.cpp
        src/optimization/optimization.cpp
        src/backend/x86_64/operand_resolver.cpp
        src/backend/x86_64/generator.cpp
        src/backend/x86_64/assembly.cpp
        src/backend/x86_64/operand.cpp
        src/utils/data.cpp
)

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
        test/snapshot/snapshot.cpp
        test/snapshot/test_snapshot.cpp
)

target_link_libraries(${PROJECT_NAME}_tests gtest gtest_main ${PROJECT_NAME}_lib)
target_compile_definitions(${PROJECT_NAME}_tests PRIVATE TEST_PATH="${CMAKE_SOURCE_DIR}/test")

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})