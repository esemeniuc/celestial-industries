cmake_minimum_required(VERSION 3.9)
project(proj)

set(CMAKE_CXX_STANDARD 11)

# where to look for cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake_modules)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(glfw3 REQUIRED)
add_subdirectory(ext/glm)

add_executable(proj
		src/objloader.cpp
		src/common.cpp
		src/main.cpp
		src/entity.cpp
		src/tile.cpp
		src/skybox.cpp
		src/world.cpp
		src/camera.cpp
		src/textureloader.cpp
		src/level.cpp
		src/config.hpp
		src/logger.cpp
		src/pathfinder.cpp
		src/renderer.cpp
		src/ai.cpp
		src/unit/genericunit.cpp
		src/unit/unitgroup.cpp
		src/building.cpp)

target_compile_options(proj PUBLIC
		-Wall
		-Wpedantic
		-Wextra
		-Wnull-dereference
		-Wshadow
		-Wformat=2
		-Wno-comment
		-Wno-unused-parameter
		-ggdb)

if (APPLE)
	target_link_libraries(proj "-framework CoreFoundation")
endif ()


target_include_directories(proj PUBLIC
		src
		src/unit
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


# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/catch2/single_include)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Make test executable
set(TEST_SOURCES
		test/example.cpp
		)

add_executable(test_suite ${TEST_SOURCES})
target_link_libraries(test_suite Catch) #can run the "test_suite" binary from the build folder

# for ctest, you can run "ctest" in the build folder
enable_testing(true)
add_test(NAME CI_Test_Suite COMMAND test_suite)