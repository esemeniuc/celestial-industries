cmake_minimum_required(VERSION 3.9)
project(proj)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(ext/glm)  #option 2

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/gl3w)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glm) #option 1
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(proj
        src/objloader.cpp
        src/common.cpp
        src/main.cpp
        src/tile.cpp
        src/world.cpp)


#target_link_libraries(proj glfw dl SDL2 SDL2_mixer) #option 1
target_link_libraries(proj glm glfw dl SDL2 SDL2_mixer) #option 2
