#pragma once
#include "common.hpp"

#include <string>
#include <vector>

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

struct OBJData {
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<unsigned int> textureCoordinateIndices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> normalIndices;
};

class OBJLoader {
public:
	static bool loadOBJ(
		std::string path, OBJData &result
	);
};

