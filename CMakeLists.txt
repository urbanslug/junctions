cmake_minimum_required(VERSION 3.14)
project(junctions)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24
# https://cmake.org/cmake/help/latest/policy/CMP0135.html
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings and AVX2 flag
    add_compile_options(-Wall -Wextra -Wpedantic -mavx2)
endif()

ADD_LIBRARY(LibsModule
  src/eds/eds.cpp
  src/eds/common.cpp
  src/core/core.cpp
  src/core/utils.cpp
  src/graph/graph.cpp
  src/graph/operations.cpp
  src/intersect/improved.cpp
  src/core/suffix_tree.cpp
  src/intersect/naive.cpp
  src/cli/cli.cpp
  src/cli/app.cpp
)

set(SOURCE_FILES
  src/main.cpp
)

# source
add_executable( junctions
${SOURCE_FILES}
)

target_link_libraries(junctions LibsModule)

set(BINARY_DIR ./bin)

file(MAKE_DIRECTORY ${BINARY_DIR}/)


add_custom_command(TARGET junctions
		   POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:junctions> ../${BINARY_DIR}/)
# ------
# Tests
# ------
add_executable( junctions_test
  tests/main_tests.cc
)


target_link_libraries(
  junctions_test
  LibsModule
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(junctions_test)
