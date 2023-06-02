cmake_minimum_required(VERSION 3.14)
project(eds)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

ADD_LIBRARY(LibsModule
  src/eds/eds.cpp
  src/core/core.cpp
  src/graph/graph.cpp
  src/core/suffix_tree.cpp
  src/utils.cpp
  src/core.cpp
)

set(SOURCE_FILES
  src/main.cpp
)

# source
add_executable( junctions
${SOURCE_FILES}
)

target_link_libraries(junctions LibsModule)


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