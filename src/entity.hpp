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
#include "model.hpp"
#include "renderer.hpp"


enum class EntityOwner {
	NONE, PLAYER, AI
};

class Entity : public Renderable {
public:
	explicit Entity(const std::shared_ptr<Renderer> &initParent);
	virtual void update(float ms);

protected:
    std::map<Config::MeshType, std::shared_ptr<Renderer>> meshRenderers;
	EntityOwner owner;
};
