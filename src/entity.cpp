#include "entity.hpp"

Entity::Entity(const std::shared_ptr<Renderer> &initParent) : Renderable(
		initParent) {}

void Entity::update(float ms)
{
}
