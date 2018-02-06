// Header
#include "tile.hpp"
#include "objloader.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>


// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

bool Tile::init(OBJ::Data obj) {

	// Clearing errors
	gl_flush_errors();

	GLuint vbo_id;
	// Vertex Buffer creation
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, obj.data.size() * sizeof(OBJ::VertexData), obj.data.data(), GL_STATIC_DRAW);

	for (auto group : obj.groups) {
		Mesh mesh;
		mesh.vbo = vbo_id;
		// Index Buffer creation
		glGenBuffers(1, &mesh.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.indices.size() * sizeof(unsigned int), group.indices.data(), GL_STATIC_DRAW);

		// Vertex Array (Container for Vertex + Index buffer)
		glGenVertexArrays(1, &mesh.vao);
		if (gl_has_errors())
			return false;

		mesh.numIndices = group.indices.size();
		mesh.material = group.material;
		meshes.push_back(mesh);
	}

	// Loading shaders
	if (!effect.load_from_file(shader_path("model.vs.glsl"), shader_path("model.fs.glsl")))
		return false;

	return true;
}

void Tile::destroy()
{
	for (auto mesh : meshes) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
		glDeleteBuffers(1, &mesh.vao);
	}

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Tile::update(float ms)
{
	// Do nothing, our buildings be static
}

void Tile::draw(glm::mat4 viewProjection)
{
	glm::mat4 mvp = viewProjection * model;

	// Setting shaders
	glUseProgram(effect.program);

	// Getting uniform locations for glUniform* calls
	GLuint mvp_uloc = glGetUniformLocation(effect.program, "mvp");
	GLuint ambient_uloc = glGetUniformLocation(effect.program, "material_ambient");
	GLuint diffuse_uloc = glGetUniformLocation(effect.program, "material_diffuse");
	GLuint specular_uloc = glGetUniformLocation(effect.program, "material_specular");
	GLuint hasDiffuseMap_uloc = glGetUniformLocation(effect.program, "hasDiffuseMap");

	for (auto mesh : meshes) {
		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		GLuint in_position_loc = glGetAttribLocation(effect.program, "in_position");
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)0); // (void*)0 because we render from the start of the stride
	
		// Diffuse map stuff
		GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
		glEnableVertexAttribArray(in_texcoord_loc); // Oddly enough we ALWAYS get texcoords, even for textureless stuff
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)sizeof(glm::vec3));

		GLuint in_normal_loc = glGetAttribLocation(effect.program, "in_normal");
		glEnableVertexAttribArray(in_normal_loc);
		glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));

		if (mesh.material.hasDiffuseMap) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.material.diffuseMap->id);
		}
		// Setting uniform values to the currently bound program
		glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);

		glUniform3f(ambient_uloc, mesh.material.ambient.x, mesh.material.ambient.y, mesh.material.ambient.z);
		glUniform3f(diffuse_uloc, mesh.material.diffuse.x, mesh.material.diffuse.y, mesh.material.diffuse.z);
		glUniform3f(specular_uloc, mesh.material.specular.x, mesh.material.specular.y, mesh.material.specular.z);
		glUniform1i(hasDiffuseMap_uloc, mesh.material.hasDiffuseMap);
		

		// Drawing!
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
	}

}

void Tile::translate(glm::vec3 translation)
{
	model = glm::translate(model, translation);
}
