cmake_minimum_required (VERSION 3.9)
project (math)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (MSVC)
    # MSVC-spezifische Flags
    add_compile_options(/W4 /permissive-)
else()
    # GCC/Clang-spezifische Flags
    add_compile_options(-g -Wall -Wextra -Wpedantic -Wl,--stack,16777216)
endif()

add_subdirectory(extern/googletest)

add_executable(math_test math_test.cc math.cc)
target_link_libraries(math_test GTest::gtest
  GTest::gtest_main)
  
enable_testing()
add_test(NAME MathTests COMMAND math_test)

