#include "objloader.hpp"
#include "common.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <regex>

namespace objloader {
	OBJLineType OBJLoader::wordToObjLineType(std::string word) {
		if (word == "#")return Comment;
		if (word == "usemtl")return UseMaterial;
		if (word == "usemap")return UseMap;
		if (word == "mtllib")return MaterialLib;
		if (word == "v")return Vertex;
		if (word == "vt")return TextureCoordinate;
		if (word == "vn")return Normal;
		if (word == "f")return Face;
		if (word == "s")return SmoothShading;

		if (word == "newmtl")return NewMaterial;
		if (word == "Ka")return Ambient;
		if (word == "Kd")return Diffuse;
		if (word == "Ks")return Specular;
		return Invalid;
	}

	bool OBJLoader::loadMaterialLibrary(std::string path, std::string filename, OBJMaterialLibrary &result) {
		std::stringstream fullPath;
		fullPath << path << filename;
		std::ifstream file;
		file.open(fullPath.str(), std::ios::in);

		if (file.fail()) {
			std::stringstream msg;
			msg << "Failed to open file " << path;
			return false;
		}
		std::string line;
		std::string name;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		bool doneWithFirst = false;
		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string firstWord;
			lineStream >> firstWord;
			OBJLineType lineType = wordToObjLineType(firstWord);
			switch (lineType) {
				case NewMaterial:
					if (doneWithFirst) {
						result.materials.push_back({
														   name, ambient, diffuse, specular
												   });
					} else {
						doneWithFirst = true;
					}
					std::getline(lineStream, name); // Gets the remainder of the steam into name
					break;
				case Ambient:
					lineStream >> ambient.x;
					lineStream >> ambient.y;
					lineStream >> ambient.z;
					break;
				case Diffuse:
					lineStream >> diffuse.x;
					lineStream >> diffuse.y;
					lineStream >> diffuse.z;
					break;
				case Specular:
					lineStream >> specular.x;
					lineStream >> specular.y;
					lineStream >> specular.z;
					break;
			}
		}
		// Push the last material to the stack
		result.materials.push_back({
										   name, ambient, diffuse, specular
								   });
		file.close();
		return true;
	}

	bool OBJLoader::loadOBJ(std::string path, std::string filename, OBJData &result) {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec3> normals;

		std::stringstream fullPath;
		fullPath << path << filename;
		std::ifstream file;
		file.open(fullPath.str(), std::ios::in);

		if (file.fail()) {
			std::stringstream msg;
			msg << "Failed to open file " << path;
			return false;
		}
		std::string line;
		OBJMaterialGroup materialGroup;
		OBJMaterialLibrary materialLibrary;
		std::string materialName;
		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string firstWord;
			lineStream >> firstWord;
			OBJLineType lineType = wordToObjLineType(firstWord);
			std::string materialLibRelativePath;
			switch (lineType) {
				case MaterialLib:
					materialLibRelativePath = "";
					lineStream >> materialLibRelativePath;
					// Get rid of useless end of the path
					loadMaterialLibrary(path, materialLibRelativePath, materialLibrary);
					materialLibRelativePath.clear();
					break;
				case UseMaterial:
					if (materialGroup.vertexIndices.size() > 0) {
						for (size_t i = 0; i < materialGroup.vertexIndices.size(); i++) {
							// OBJ index starts at 1 ಠ_ಠ
							materialGroup.vertexIndices[i]--;
							materialGroup.normalIndices[i]--;
							materialGroup.textureCoordinateIndices[i]--;
						}
						result.groups.push_back(materialGroup);
					}
					materialGroup.vertexIndices.clear();
					materialGroup.textureCoordinateIndices.clear();
					materialGroup.normalIndices.clear();
					std::getline(lineStream, materialName);
					for (auto material : materialLibrary.materials) {
						if (material.name == materialName) {
							materialGroup.material = material;
							break;
						}
					}
					break;
				case Vertex:
					glm::vec3 vertex;
					lineStream >> vertex.x;
					lineStream >> vertex.y;
					lineStream >> vertex.z;
					result.vertices.push_back(vertex);
					break;
				case TextureCoordinate:
					glm::vec2 textureCoordinate;
					lineStream >> textureCoordinate.x;
					lineStream >> textureCoordinate.y;
					result.textureCoordinates.push_back(textureCoordinate);
					break;
				case Normal:
					glm::vec3 normal;
					lineStream >> normal.x;
					lineStream >> normal.y;
					lineStream >> normal.z;
					result.normals.push_back(normal);
					break;
				case Face:
					for (size_t i = 0; i < 3; i++) {
						std::string faceGroup;
						lineStream >> faceGroup;
						unsigned int vertexIndex = 0, textureCoordinateIndex = 0, normalIndex = 0;
						// Kindda hacky
						sscanf(faceGroup.c_str(), "%d/%d/%d", &vertexIndex, &textureCoordinateIndex, &normalIndex);
						materialGroup.vertexIndices.push_back(vertexIndex);
						materialGroup.textureCoordinateIndices.push_back(textureCoordinateIndex);
						materialGroup.normalIndices.push_back(normalIndex);
					}
					break;
			}
		}

		// Gotta get that straggler
		for (size_t i = 0; i < materialGroup.vertexIndices.size(); i++) {
			// OBJ index starts at 1 ಠ_ಠ
			materialGroup.vertexIndices[i]--;
			materialGroup.normalIndices[i]--;
			materialGroup.textureCoordinateIndices[i]--;
		}
		result.groups.push_back(materialGroup);
		file.close();
		return true;
	}
}