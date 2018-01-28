#include "entity.hpp";

void Entity::set_velocity(glm::vec3 v) 
{
	this->velocity.x = v.x;
	this->velocity.y = v.y;
	this->velocity.z = v.z;
}

void Entity::set_gravity(glm::vec3 g) 
{
	this->gravity.x = g.x;
	this->gravity.y = g.y;
	this->gravity.z = g.z;
}

void Entity::set_force(glm::vec3 f) 
{
	this->applied_force.x = f.x;
	this->applied_force.y = f.y;
	this->applied_force.z = f.z;
}

void Entity::set_geometryId(long id) 
{
	this->geometry_id = id;
}

void Entity::set_collision_geometry_type(collision_geometry_type cgtype) 
{
	this->cg_type = cgtype;
}

collision_geometry_type Entity::get_collision_geometry_type() 
{
	return this->cg_type;
}

glm::vec3 Entity::get_velocity() 
{
	return this->velocity;
}

long Entity::get_geometryId() 
{
	return this->geometry_id;
}

bool Entity::is_textured() 
{
	return this->texture_flag;
}