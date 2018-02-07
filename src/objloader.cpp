#include "objloader.hpp"
#include "common.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <regex>

namespace OBJ {
	LineType Loader::wordToObjLineType(std::string word) {
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
		if (word == "map_Ka")return AmbientMap;
		if (word == "map_Kd")return DiffuseMap;
		return Invalid;
	}

	bool Loader::loadMaterialLibrary(std::string path, std::string filename, MaterialLibrary &result) {
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
		bool hasDiffuseMap = false;
		std::shared_ptr<Texture> diffuseMap;
		bool doneWithFirst = false;
		std::string texturePath;
		std::vector<std::string> texturePathParts; // Did I mention the tex files can be in arbitrary relative positions :)
		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string firstWord;
			lineStream >> firstWord;
			LineType lineType = wordToObjLineType(firstWord);
			switch (lineType) {
				case NewMaterial:
					if (doneWithFirst) {
						result.materials.push_back({
							name,
							ambient,
							diffuse,
							specular,
							hasDiffuseMap,
							diffuseMap
							});
						hasDiffuseMap = false; // If we don't reset this EVERYTHING has it on true
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
				case DiffuseMap:
					hasDiffuseMap = true;
					texturePath = "";
					std::getline(lineStream, texturePath);
					texturePath.erase(0, 1); // We pick up a space at the start for no good reason
					texturePathParts = splitString(texturePath, '/');
					texturePath = pathAppender(path, texturePathParts);
					diffuseMap = std::make_shared<Texture>();

					bool loadSuccesfull = diffuseMap->load_from_file(texturePath.c_str());
					if (!diffuseMap->is_valid()  || !loadSuccesfull) {
						std::cout << "Failed to load diffuse map: \n" << texturePath << std::endl;
						return false;
					}
					texturePath.clear();
			}
		}
		// Push the last material to the stack
		result.materials.push_back({
			name,
			ambient,
			diffuse,
			specular,
			hasDiffuseMap,
			diffuseMap,
			});
		file.close();
		return true;
	}

	bool Loader::loadOBJ(std::string path, std::string filename, Data &result) {
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
		MaterialGroup materialGroup;
		MaterialLibrary materialLibrary;
		std::string materialName;

		std::vector<VertexData> vertexData;
		std::vector<unsigned int> materialIndices;

		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string firstWord;
			lineStream >> firstWord;
			LineType lineType = wordToObjLineType(firstWord);
			std::string materialLibRelativePath;
			switch (lineType) {
				case MaterialLib:
					materialLibRelativePath = "";
					lineStream >> materialLibRelativePath;
					if (!loadMaterialLibrary(path, materialLibRelativePath, materialLibrary)) {
						std::cout << "Failed to load material library " << materialLibRelativePath << std::endl;
						return false;
					}
					materialLibRelativePath.clear();
					break;
				case UseMaterial:
					if (materialIndices.size() > 0) {
						materialGroup.indices = materialIndices;
						materialIndices.clear();
						result.groups.push_back(materialGroup);
					}
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
						if (textureCoordinates.size() != 0) {
							sscanf(faceGroup.c_str(), "%d/%d/%d", &vertexIndex, &textureCoordinateIndex, &normalIndex);
							vertexData.push_back({
								vertices[vertexIndex - 1],
								textureCoordinates[textureCoordinateIndex - 1],
								normals[normalIndex - 1]
							});
						}
						else {
							sscanf(faceGroup.c_str(), "%d//%d", &vertexIndex, &normalIndex);
							vertexData.push_back({
								vertices[vertexIndex - 1],
								glm::vec2(0.0f, 0.0f),
								normals[normalIndex - 1]
							});
						}
						
						materialIndices.push_back(vertexData.size() - 1);
					}
					break;
			}
		}

		materialGroup.indices = materialIndices;
		result.groups.push_back(materialGroup);
		result.data = vertexData;

		file.close();
		return true;
	}
}