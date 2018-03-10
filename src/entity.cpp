#include "entity.hpp"

Entity::Entity(Model::MeshType geometry) : renderable(Model::meshRenderers[geometry]){}

void Entity::update(float ms)
{
    // TODO: add code to update an entity
}
