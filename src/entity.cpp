#include "entity.hpp"

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

// specify angles in degrees
void Entity::set_rotation(glm::vec3 rotation_angles_degrees)
{
	this->rotation = rotation_angles_degrees;	
}

void Entity::set_translation(glm::vec3 _translation)
{
	this->translation = _translation;
}

void Entity::set_scale(glm::vec3 _scale)
{
	this->scale = _scale;
}

glm::mat4 Entity::model_matrix()
{	
	// build rotation matrix using euler angles and quaternions
	glm::mat4 rotation_matrix;
	glm::quat rotation_quaternion(this->rotation);
	rotation_matrix = glm::toMat4(rotation_quaternion);

	// build translation matrix
	glm::mat4 translation_matrix = glm::translate(glm::mat4(),this->translation);

	//build scaling matrix
	glm::mat4 scaling_matrix = glm::scale(this->scale);

	// order of transformations is important: from right to left
	// scale first, then rotate and translate
	glm::mat4 model_matrix = glm::mat4(1.f);
	model_matrix = translation_matrix * rotation_matrix * scaling_matrix;
	
	return model_matrix;
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