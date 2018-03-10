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

set(CUSTOM_COMPILE_FLAGS
		-Wall
		-Wpedantic
		-Wextra
		-Wnull-dereference
		-Wshadow
		-Wformat=2
		-Wno-comment
		-Wno-unused-parameter
		-ggdb
		)

set(CUSTOM_INCLUDE_DIRS
		src
		src/unit
		ext/gl3w
		ext/gl3w/GL
		ext/stb_image
		${SDL2_INCLUDE_DIRS}
		${SDL2_MIXER_INCLUDE_DIRS}
		)

set(CUSTOM_INCLUDE_LIBS
		OpenGL::GL
		glfw
		glm
		${CMAKE_DL_LIBS}
		${SDL2_LIBRARIES}
		${SDL2_MIXER_LIBRARIES}
		)

set(PROJ_SOURCES
		ext/gl3w/GL/gl3w.c
		src/ai.cpp
		src/camera.cpp
		src/config.hpp
		src/common.cpp
		src/entity.cpp
		src/global.cpp
		src/level.cpp
		src/logger.cpp
		src/model.cpp
		src/objloader.cpp
		src/pathfinder.cpp
		src/renderer.cpp
		src/shader.cpp
		src/skybox.cpp
		src/textureloader.cpp
		src/tile.cpp
		src/world.cpp
		src/unit/genericunit.cpp
		src/unit/unitgroup.cpp
		src/building.cpp
		src/global.hpp)


add_executable(proj
		src/main.cpp
		${PROJ_SOURCES})
target_compile_options(proj PUBLIC ${CUSTOM_COMPILE_FLAGS})

if (APPLE)
	target_link_libraries(proj "-framework CoreFoundation")
endif ()

target_include_directories(proj PUBLIC ${CUSTOM_INCLUDE_DIRS})
target_link_libraries(proj ${CUSTOM_INCLUDE_LIBS})


# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/catch2/single_include)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

set(TEST_SUITE_SOURCES
		test/example.cpp
		test/genericunit_test.cpp
		)

add_executable(test_suite ${PROJ_SOURCES} ${TEST_SUITE_SOURCES})
target_compile_options(test_suite PUBLIC ${CUSTOM_COMPILE_FLAGS})
target_include_directories(test_suite PUBLIC ${CUSTOM_INCLUDE_DIRS})
#target_link_libraries(test_suite ) #can run the "test_suite" binary from the build folder
target_link_libraries(test_suite Catch)
target_link_libraries(test_suite ${CUSTOM_INCLUDE_LIBS})

# for ctest, you can run "ctest" in the build folder
enable_testing(true)
add_test(NAME CI_Test_Suite COMMAND test_suite)