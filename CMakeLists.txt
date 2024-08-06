cmake_minimum_required(VERSION 3.28)

project(arkoi_language)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
add_executable(arkoi_language src/main.cpp src/scanner.cpp src/token.cpp
        src/cursor.cpp
        include/cursor.h)