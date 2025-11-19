cmake_minimum_required(VERSION 3.22)
project(opengl-renderer LANGUAGES CXX)

# use C++23 and prefer static linking
set(CMAKE_CXX_STANDARD 23)
set(BUILD_SHARED_LIBS OFF)

# create executable
add_executable(${PROJECT_NAME} "src/main.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE "include/")
target_precompile_headers(${PROJECT_NAME} PRIVATE "src/pch.hpp")
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

# add dependencies
include("cmake/opengl.cmake")
include("cmake/sdl.cmake")