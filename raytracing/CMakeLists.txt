cmake_minimum_required (VERSION 3.9)
project (geometry)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g -Wall -Wextra -Wpedantic -Wl,--stack,16777216)

add_subdirectory(
    "D:/Dokumente/workspace/workspace_computergrafik/extern/googletest"
    "${CMAKE_BINARY_DIR}/googletest"
)

find_package(SDL2 REQUIRED)


add_executable(raytracer raytracer.cc geometry.cc math.cc Screen.cc Camera.cc)
target_link_libraries(raytracer SDL2::SDL2)

