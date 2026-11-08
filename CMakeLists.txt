cmake_minimum_required(VERSION 3.12)
project(toyc_lexer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(compiler src/main.cpp)
target_compile_options(compiler PRIVATE -Wall -Wextra -Wpedantic)
# The evaluator invokes ./compiler -token at the repository root.
# Always compile the executable to that exact location.
set_target_properties(compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
