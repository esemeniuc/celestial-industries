// Header
#include "tile.hpp"
#include "objloader.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>


// glm
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"

bool Tile::init() {

	std::string path = "D:\\OneDrive\\UBC\\CPSC\\436D\\Models\\pineTree.obj";
	OBJData obj;
	if (!OBJLoader::loadOBJ(path, obj))return false;

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(vec3), obj.vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.vertexIndices.size()*sizeof(unsigned int), obj.vertexIndices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("model.vs.glsl"), shader_path("model.fs.glsl")))
		return false;

	mesh.numIndices = obj.vertexIndices.size();
	return true;
}

void Tile::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Tile::draw(glm::mat4 mvp)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	//transform_begin();
	//transform_translate(m_position);
	//transform_rotate(m_rotation);
	//transform_scale(m_scale);/*
	//transform_end();*/


	// Setting shaders
	glUseProgram(effect.program);

	// Getting uniform locations for glUniform* calls
	GLuint mvp_uloc = glGetUniformLocation(effect.program, "mvp");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLuint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0); // (void*)0 because we render the whole array

																						   // Setting uniform values to the currently bound program
	glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);

	// Drawing!
	glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
}