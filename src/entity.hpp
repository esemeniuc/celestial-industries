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

class Entity {
public:
    Entity();
	virtual void update(float ms);
protected:
    Renderable renderable;
};
