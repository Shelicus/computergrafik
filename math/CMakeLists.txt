cmake_minimum_required (VERSION 3.9)
project (math)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(
    "D:/Dokumente/workspace/workspace_computergrafik/extern/googletest"
    "${CMAKE_BINARY_DIR}/googletest"
)

add_executable(math_test math_test.cc math.cc)
target_link_libraries(math_test GTest::gtest
  GTest::gtest_main)
  
enable_testing()
add_test(NAME MathTests COMMAND math_test)

