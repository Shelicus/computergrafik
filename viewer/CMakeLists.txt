cmake_minimum_required (VERSION 3.9)
project (viewer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g -Wall -Wextra -Wpedantic -Wl,--stack,16777216)

add_subdirectory(
    "D:/Dokumente/workspace/workspace_computergrafik/extern/googletest"
    "${CMAKE_BINARY_DIR}/googletest"
)

find_package(SDL2 REQUIRED)


find_package(SDL2_mixer REQUIRED)


add_executable(viewer viewer.cc wavefront.cc)
target_link_libraries(viewer SDL2::SDL2 OPENGL32 GLEW32) # MinGW
#target_link_libraries(viewer SDL2::SDL2 GL GLEW)

add_executable(wavefront_test wavefront.cc wavefront_test.cc)
target_link_libraries(wavefront_test gtest gtest_main)

