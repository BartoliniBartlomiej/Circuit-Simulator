cmake_minimum_required(VERSION 3.16)
project(circuit_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ostrzeżenia – pomogą przy rozbudowie
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(circuit_sim
    src/main.cpp
    src/core/Circuit.cpp
    # Node.hpp i Component.hpp są header-only na razie
)

target_include_directories(circuit_sim PRIVATE src)