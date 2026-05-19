cmake_minimum_required(VERSION 3.16)
project(circuit_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- Biblioteka core (współdzielona między app i testy) ---
add_library(circuit_core
    src/core/Circuit.cpp
)
target_include_directories(circuit_core PUBLIC src)

# --- Główna aplikacja ---
add_executable(circuit_sim
    src/main.cpp
)
target_link_libraries(circuit_sim PRIVATE circuit_core)

# --- Testy ---
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Nie instaluj gtest globalnie w systemie
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(circuit_tests
    tests/test_resistor.cpp
    tests/test_voltage_source.cpp
    tests/test_current_source.cpp
)
target_link_libraries(circuit_tests PRIVATE circuit_core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(circuit_tests)