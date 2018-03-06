#include "entity.hpp"

void Entity::setVelocity(glm::vec3 v)
{
	this->velocity = v;
}

void Entity::setGravity(glm::vec3 g) 
{
	this->gravity = g;
}

void Entity::setForce(glm::vec3 f) 
{
	this->applied_force = f;
}

void Entity::setGeometryId(long id) 
{
	this->geometry_id = id;
}

// specify angles in radians
void Entity::setRotation(glm::vec3 rotation_angles_radians)
{
	this->rotation = rotation_angles_radians;
}

void Entity::setTranslation(glm::vec3 _translation)
{
	this->translation = _translation;
}

void Entity::setScale(glm::vec3 _scale)
{
	this->scale = _scale;
}

void Entity::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

void Entity::getCameraPosition(glm::vec3 cameraPos)
{
	this->cameraPosition = cameraPos;
}

glm::vec3 Entity::getPosition()
{
	return this->position;
}

void Entity::applyTransformations()
{	
	// apply scaling
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, this->scale);
	// apply rotations
	model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
	// apply translations
	model = glm::translate(model, this->translation);
	setModelMatrix(0, model);
}

void Entity::setCollisionGeometryType(collision_geometry_type cgtype) 
{
	this->cg_type = cgtype;
}

glm::mat4 Entity::getModelMatrix()
{
	return this->model;
}

collision_geometry_type Entity::getCollisionGeometryType() 
{
	return this->cg_type;
}

glm::vec3 Entity::getVelocity() 
{
	return this->velocity;
}

long Entity::getGeometryId() 
{
	return this->geometry_id;
}

bool Entity::isTextured() 
{
	return this->texture_flag;
}

Entity::Entity(const std::shared_ptr<Renderer> &initParent) : Renderable(
		initParent) {}
