#pragma once

// glm
#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace objloader {
	enum OBJLineType {
		Invalid,
		Comment,
		UseMaterial,
		UseMap,
		MaterialLib,
		Vertex,
		TextureCoordinate,
		Normal,
		Face,
		SmoothShading,

		// Stuff from associated .mtl files
		NewMaterial,
		Ambient,
		Diffuse,
		Specular,
	};

	struct OBJMaterial {
		std::string name;
		glm::vec3 ambient, diffuse, specular;
	};

	struct OBJMaterialLibrary {
		std::vector<OBJMaterial> materials;
	};

	struct OBJMaterialGroup {
		OBJMaterial material;
		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> textureCoordinateIndices;
		std::vector<unsigned int> normalIndices;
	};

	struct OBJData {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec3> normals;
		std::vector<OBJMaterialGroup> groups;
	};

	class OBJLoader {
	public:
		static bool loadOBJ(
				std::string path, std::string filename, OBJData &result
		);

		static OBJLineType wordToObjLineType(std::string word);

		static bool loadMaterialLibrary(
				std::string path, std::string filename, OBJMaterialLibrary &result
		);
	};
}