#pragma once

// glm
#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace OBJ {
	enum LineType {
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

	struct Material {
		std::string name;
		glm::vec3 ambient, diffuse, specular;
	};

	struct MaterialLibrary {
		std::vector<Material> materials;
	};

	struct MaterialGroup {
		Material material;
		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> textureCoordinateIndices;
		std::vector<unsigned int> normalIndices;
	};

	struct Data {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec3> normals;
		std::vector<MaterialGroup> groups;
	};

	class Loader {
	public:
		static bool loadOBJ(
				std::string path, std::string filename, Data &result
		);

		static LineType wordToObjLineType(std::string word);

		static bool loadMaterialLibrary(
				std::string path, std::string filename, MaterialLibrary &result
		);
	};
}