cmake_minimum_required(VERSION 3.9)
project(proj)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(glfw3 3.2 REQUIRED)
add_subdirectory(ext/glm)

add_executable(proj
        src/objloader.cpp
        src/common.cpp
        src/main.cpp
        src/tile.cpp
        src/world.cpp
		src/camera.cpp
		src/textureloader.cpp)

target_include_directories(proj PUBLIC
        ext/gl3w
        ext/stb_image
        ${SDL_INCLUDE_DIR}
        ${SDL_MIXER_INCLUDE_DIRS}
        )

target_link_libraries(proj
        OpenGL::GL
        glfw
        glm
        ${CMAKE_DL_LIBS}
        ${SDL_LIBRARY}
        ${SDL_MIXER_LIBRARIES})