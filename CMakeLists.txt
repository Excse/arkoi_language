cmake_minimum_required(VERSION 3.29.6)

project(arkoi_language VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# =====================
# Google Test
# =====================

FetchContent_Declare(
        google_test
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gmock_build_tests OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(google_test)

# =====================
# Library
# =====================

include_directories(include)

add_library(${PROJECT_NAME}_lib
        src/front/token.cpp
        src/front/scanner.cpp
        src/front/parser.cpp
        src/sem/name_resolver.tpp
        src/sem/name_resolver.cpp
        src/sem/symbol_table.tpp
        src/sem/symbol.cpp
        src/sem/type_resolver.cpp
        src/sem/type.cpp
        src/il/instruction.cpp
        src/il/generator.cpp
        src/il/il_printer.cpp
        src/il/cfg_printer.cpp
        src/il/operand.cpp
        src/il/cfg.cpp
        src/opt/pass.cpp
        src/opt/pass.tpp
        src/opt/constant_folding.cpp
        src/utils/dep_graph.tpp
        src/utils/size.cpp
)

target_link_libraries(${PROJECT_NAME}_lib)

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
        test/test_dep_graph.cpp
)

target_link_libraries(${PROJECT_NAME}_tests gtest gtest_main gmock gmock_main ${PROJECT_NAME}_lib)
target_compile_definitions(${PROJECT_NAME}_tests PRIVATE TEST_PATH="${CMAKE_SOURCE_DIR}/test")

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})