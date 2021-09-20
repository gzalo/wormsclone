cmake_minimum_required(VERSION 3.17)
project(wormsclone)

set (CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(wormsclone worms.cpp graphics.cpp graphics.h Character.cpp Character.h Bullet.cpp Bullet.h Game.cpp Game.h)
target_link_libraries(wormsclone ${SDL2_LIBRARIES} ${OPENGL_LIBRARY})
