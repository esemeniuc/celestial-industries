#pragma once

#include "textureloader.hpp"
#include <memory>

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
		AmbientMap, // Not supported
		DiffuseMap,
	};

	struct Material {
		std::string name;
		glm::vec3 ambient, diffuse, specular;

		// We could avoid this bool since GLID is 0 if not initialized.
		// It is however nice to know the difference between 
		// "There is no texture, and there shouldn't be"
		// and "Where the fuck is my texture"
		bool hasDiffuseMap;
		std::shared_ptr<Texture> diffuseMap;
		//Texture diffuseMap;
	};

	struct VertexData {
		// USed to group the data so it can all be fed into the GPU
		glm::vec3 position;
		glm::vec2 texcoord;
		glm::vec3 normal;
	};

	struct MaterialLibrary {
		std::vector<Material> materials;
	};

	struct MaterialGroup {
		Material material;
		std::vector<unsigned int> indices;
	};

	struct Data {
		std::vector<VertexData> data;
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