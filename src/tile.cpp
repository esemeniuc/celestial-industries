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

enum ObjLineType {
	Invalid,
	Comment,
	UseMaterial,
	MaterialLib,
	Vertex,
	TextureCoordinate,
	Normal,
	Face,
	SmoothShading,
};

ObjLineType wordToObjLineType(std::string word) {
	if (word == "#")return Comment;
	if (word == "usemtl")return UseMaterial;
	if (word == "mtllib")return MaterialLib;
	if (word == "v")return Vertex;
	if (word == "vt")return TextureCoordinate;
	if (word == "vn")return Normal;
	if (word == "f")return Face;
	if (word == "s")return SmoothShading;
	
	return Invalid;
}

bool Tile::init() {

	std::vector< unsigned int > vertexIndices, textureCoordinateIndices, normalIndices;
	std::vector< vec3 > temp_vertices;
	std::vector< vec2 > temp_textureCoordinates;
	std::vector< vec3 > temp_normals;
	std::string path = "D:\\OneDrive\\UBC\\CPSC\\436D\\Models\\pineTree.obj";
	std::ifstream file;
	file.open(path, std::ios::in);

	if (file.fail()) {
		std::cout << "Failed to open file " << path << std::endl;
		return false;
	}
	std::vector<std::string> firstWords;
	std::string line;
	size_t iteration = 0;
	while (std::getline(file, line)) {
		iteration++;
		std::cout << iteration << std::endl;
		std::istringstream lineStream(line);
		std::string firstWord;
		lineStream >> firstWord;
		firstWords.push_back(firstWord);
		ObjLineType lineType = wordToObjLineType(firstWord);
		switch (lineType) {
		case Vertex:
			vec3 vertex;
			lineStream >> vertex.x;
			lineStream >> vertex.y;
			lineStream >> vertex.z;
			temp_vertices.push_back(vertex);
			break;
		case TextureCoordinate:
			vec2 textureCoordinate;
			lineStream >> textureCoordinate.x;
			lineStream >> textureCoordinate.y;
			temp_textureCoordinates.push_back(textureCoordinate);
			break;
		case Normal:
			vec3 normal;
			lineStream >> normal.x;
			lineStream >> normal.y;
			lineStream >> normal.z;
			temp_normals.push_back(normal);
			break;
		case Face:
			for (size_t i = 0; i < 3; i++) {
				std::string faceGroup;
				lineStream >> faceGroup;
				unsigned int vertexIndex=0, textureCoordinateIndex=0, normalIndex=0;
				// Kindda hacky
				sscanf(faceGroup.c_str(), "%d/%d/%d", &vertexIndex, &textureCoordinateIndex, &normalIndex);
				vertexIndices.push_back(vertexIndex);
				textureCoordinateIndices.push_back(textureCoordinateIndex);
				normalIndices.push_back(normalIndex);
			}
			break;
		}
	}

	std::vector < vec3 > resultVertices;
	std::vector < vec2 > resultTextureCoordinates;
	std::vector < vec3 > resultNormals;
	if (vertexIndices.size() != normalIndices.size() || vertexIndices.size() != textureCoordinateIndices.size()) {
		std::cout << "OBJ PARSE ERROR: Invalid face parsing occured" << std::endl;
	}
	for (size_t i = 0; i < vertexIndices.size(); i++) {
		size_t vertexIndex = vertexIndices[i];
		vec3 vertex = temp_vertices[vertexIndex - 1]; // OBJ index starts at 1 ಠ_ಠ
		resultVertices.push_back(vertex);
		vertexIndices[i]--;
		
		size_t textureCoordinateIndex = textureCoordinateIndices[i];
		vec2 textureCoordinate = temp_textureCoordinates[textureCoordinateIndex - 1];
		resultTextureCoordinates.push_back(textureCoordinate);

		size_t normalIndex = normalIndices[i];
		vec3 normal = temp_normals[normalIndex - 1];
		resultNormals.push_back(normal);
	}

	file.close();

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, temp_vertices.size() * sizeof(vec3), temp_vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size()*sizeof(unsigned int), vertexIndices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("model.vs.glsl"), shader_path("model.fs.glsl")))
		return false;

	m_indices = vertexIndices.size();
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

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

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
	glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, nullptr);
}