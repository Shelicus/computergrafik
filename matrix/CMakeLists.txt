cmake_minimum_required (VERSION 3.9)
project (matrix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g -Wall -Wextra -Wpedantic -Wl,--stack,16777216)

add_subdirectory(
    "D:/Dokumente/workspace/workspace_computergrafik/extern/googletest"
    "${CMAKE_BINARY_DIR}/googletest"
)


enable_testing()
add_executable(math_test math_test.cc math.cc)
target_link_libraries(math_test GTest::gtest GTest::gtest_main)
add_executable(matrix_test matrix_test.cc matrix.cc math.cc)
target_link_libraries(matrix_test GTest::gtest GTest::gtest_main)

