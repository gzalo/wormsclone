cmake_minimum_required(VERSION 3.17)
project(wormsclone)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(wormsclone worms.cpp)
target_link_libraries(wormsclone ${SDL2_LIBRARIES} ${OPENGL_LIBRARY})
