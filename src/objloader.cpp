#include "objloader.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

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

bool OBJLoader::loadOBJ(std::string path, OBJData &result) {
	std::vector< unsigned int > vertexIndices, textureCoordinateIndices, normalIndices;
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > textureCoordinates;
	std::vector< glm::vec3 > normals;

	std::ifstream file;
	file.open(path, std::ios::in);

	if (file.fail()) {
		std::stringstream msg;		
		msg << "Failed to open file " << path;
		return false;
	}
	std::string line;
	size_t iteration = 0;
	while (std::getline(file, line)) {
		iteration++;
		std::cout << iteration << std::endl;
		std::istringstream lineStream(line);
		std::string firstWord;
		lineStream >> firstWord;
		ObjLineType lineType = wordToObjLineType(firstWord);
		switch (lineType) {
		case Vertex:
			glm::vec3 vertex;
			lineStream >> vertex.x;
			lineStream >> vertex.y;
			lineStream >> vertex.z;
			vertices.push_back(vertex);
			break;
		case TextureCoordinate:
			glm::vec2 textureCoordinate;
			lineStream >> textureCoordinate.x;
			lineStream >> textureCoordinate.y;
			textureCoordinates.push_back(textureCoordinate);
			break;
		case Normal:
			glm::vec3 normal;
			lineStream >> normal.x;
			lineStream >> normal.y;
			lineStream >> normal.z;
			normals.push_back(normal);
			break;
		case Face:
			for (size_t i = 0; i < 3; i++) {
				std::string faceGroup;
				lineStream >> faceGroup;
				unsigned int vertexIndex = 0, textureCoordinateIndex = 0, normalIndex = 0;
				// Kindda hacky
				sscanf(faceGroup.c_str(), "%d/%d/%d", &vertexIndex, &textureCoordinateIndex, &normalIndex);
				vertexIndices.push_back(vertexIndex);
				textureCoordinateIndices.push_back(textureCoordinateIndex);
				normalIndices.push_back(normalIndex);
			}
			break;
		}
	}

	if (vertexIndices.size() != normalIndices.size() || vertexIndices.size() != textureCoordinateIndices.size()) {
		std::cout << "OBJ PARSE ERROR: Invalid face parsing occured" << std::endl;
	}
	for (size_t i = 0; i < vertexIndices.size(); i++) {
		// OBJ index starts at 1 ಠ_ಠ
		vertexIndices[i]--;
		normalIndices[i]--;
		textureCoordinateIndices[i]--;
	}

	file.close();
	result.vertices = vertices;
	result.vertexIndices = vertexIndices;
	result.normals = normals;
	result.normalIndices = normalIndices;
	result.textureCoordinates = textureCoordinates;
	result.textureCoordinateIndices = textureCoordinateIndices;
	return true;
}