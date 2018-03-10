#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <cstring>
#include <map>

// glm
#include "glm/glm.hpp"

// custom headers
#include "objloader.hpp"
#include "renderer.hpp"
#include "model.hpp"

class Entity {
public:
    Entity() = default;
    Entity(Model::MeshType geometry);
	virtual void update(float ms);

protected:
    Renderable renderable;
};
