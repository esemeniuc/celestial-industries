#pragma once
#include "entity.hpp"
class Skybox : public Entity
{
public:
	bool init(OBJ::Data obj);

	void destroy();

	void update(float ms);

	void draw(glm::mat4 mvp)override;	
};

