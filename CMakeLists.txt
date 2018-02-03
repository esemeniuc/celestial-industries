cmake_minimum_required(VERSION 3.9)
project(proj)

set(CMAKE_CXX_STANDARD 11)

# where to look for cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake_modules)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(glfw3 3.2 REQUIRED)
add_subdirectory(ext/glm)

add_executable(proj
        src/objloader.cpp
        src/common.cpp
        src/main.cpp
        src/entity.cpp
        src/tile.cpp
        src/skybox.cpp
        src/world.cpp)

target_compile_options(proj PUBLIC
    -Wall
    -Wpedantic
    -Wextra
    -Wlogical-op
    -Wnull-dereference
    -Wshadow
    -Wformat=2
    -Wno-comment
    -Wno-unused-parameter
    -ggdb)

target_include_directories(proj PUBLIC
        ext/gl3w
        ext/stb_image
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        )

target_link_libraries(proj
        OpenGL::GL
        glfw
        glm
        ${CMAKE_DL_LIBS}
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        )