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

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
	# Support Unix Makefiles and Ninja
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif ()

set(CUSTOM_COMPILE_FLAGS
		-Wall
		-Wpedantic
		-Wextra
		-Wnull-dereference
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
		ext/gl3w/GL/gl3w.cpp
		src/aimanager.cpp
		src/building.cpp
		src/camera.cpp
		src/collisiondetection.cpp
		src/collisiondetector.cpp
		src/collisionResolver.cpp
		src/config.hpp
		src/common.cpp
		src/entity.cpp
		src/global.cpp
		src/global.hpp
		src/level.cpp
		src/logger.cpp
		src/model.cpp
		src/objloader.cpp
		src/pathfinder.cpp
		src/particle.cpp
		src/renderer.cpp
		src/rigidBody.cpp
		src/shader.cpp
		src/skybox.cpp
		src/textureloader.cpp
		src/tile.cpp
		src/world.cpp
		src/unit/gamepiece.cpp
		src/unit/gameunit.cpp
		src/unit/unitmanager.cpp src/unit/unitmanager.hpp)

add_library(libproj ${PROJ_SOURCES})
target_compile_options(libproj PUBLIC ${CUSTOM_COMPILE_FLAGS})
target_link_libraries(libproj PUBLIC ${CUSTOM_INCLUDE_LIBS})
target_include_directories(libproj PUBLIC ${CUSTOM_INCLUDE_DIRS})

set(TEST_SUITE_SOURCES
		test/example.cpp
		test/genericunit_test.cpp
		)

add_executable(proj src/main.cpp) #main.cpp gets compile flags through PUBLIC flag of libproj

if (APPLE)
	target_link_libraries(libproj "-framework CoreFoundation")
endif ()

target_link_libraries(proj libproj)


# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/catch2/single_include)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
add_executable(test_suite ${TEST_SUITE_SOURCES})
target_link_libraries(test_suite Catch libproj) #so we can run the "test_suite" binary from the build folder

# for ctest, you can run "ctest" in the build folder
enable_testing(true)
add_test(NAME CI_Test_Suite COMMAND test_suite)