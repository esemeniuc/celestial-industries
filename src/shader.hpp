#pragma once

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

#include "common.hpp"

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Shader
{
    bool load_from_file(const char* vs_path, const char* fs_path);
    void release();

    GLuint vertex;
    GLuint fragment;
    GLuint program;
};