#pragma once
#include "common.hpp"

#include <vector>

bool loadOBJ(
	const char* path,
	std::vector<vec3>& out_vertices,
	std::vector<vec3>& out_uvs,
	std::vector<vec3>& out_normals
)