cmake_minimum_required(VERSION 3.16)
project(sagitta CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# everything except main.cpp goes into a library, so the tests can use it too
file(GLOB SAGITTA_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM SAGITTA_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_library(sagitta_core ${SAGITTA_SOURCES})
target_include_directories(sagitta_core PUBLIC src)
target_compile_options(sagitta_core PRIVATE -Wall -Wextra -pedantic)

add_executable(sagitta src/main.cpp)
target_link_libraries(sagitta PRIVATE sagitta_core)

# GoogleTest is downloaded the first time you build
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/unit/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE sagitta_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
