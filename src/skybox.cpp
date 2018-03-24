#include "skybox.hpp"

bool Skybox::init(OBJ::Data obj)
{
	// Clearing errors
	gl_flush_errors();

	GLuint vbo_id;
	// Vertex Buffer creation
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, obj.data.size() * sizeof(OBJ::VertexData), obj.data.data(), GL_STATIC_DRAW);

    meshes = std::make_shared<std::vector<Mesh>>();
	for (auto group : obj.groups) {
		Mesh mesh;
		mesh.vbo = vbo_id;
		// Index Buffer creation
		glGenBuffers(1, &mesh.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.indices.size() * sizeof(unsigned int), group.indices.data(), GL_STATIC_DRAW);

		// Vertex Array (Container for Vertex + Index buffer)
		glGenVertexArrays(1, &mesh.vao);
		if (gl_has_errors())
			return false;

		mesh.numIndices = group.indices.size();
		mesh.material = group.material;
		meshes->push_back(mesh);
	}

	// Loading shaders
    shader = std::make_shared<Shader>();
	return shader->load_from_file(shader_path("skybox.vs.glsl"), shader_path("skybox.fs.glsl"));
}

void Skybox::destroy()
{
	for (auto mesh : meshes) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
		glDeleteBuffers(1, &mesh.vao);
	}

	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
	glDeleteShader(shader->program);
}

void Skybox::update(float ms)
{
	// try to animate skybox later, i.e. make it rotate
}

void Skybox::draw(glm::mat4 viewProjection)
{
	// Setting shaders
	glUseProgram(shader->program);

	// Getting uniform locations for glUniform* calls
	GLuint vp_uloc = glGetUniformLocation(shader->program, "viewProjection");
	GLuint model_uloc = glGetUniformLocation(shader->program, "model");
	GLuint camera_uloc = glGetUniformLocation(shader->program, "cameraPosition");

	for (auto mesh : meshes) {
		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		GLuint position_loc = glGetAttribLocation(shader->program, "position");
		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)0); 
		glUniformMatrix4fv(vp_uloc, 1, GL_FALSE, &viewProjection[0][0]);
		glUniformMatrix4fv(model_uloc, 1, GL_FALSE, &this->model[0][0]);
		glUniform3fv(camera_uloc, 1, &(this->cameraPosition[0]));
		
		// Drawing!
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
	}
}

void Skybox::set_cube_faces(const std::string relative_path)
{
	faces.front = relative_path + "/front.png";
	faces.back = relative_path + "/back.png";
	faces.left = relative_path + "/left.png";
	faces.right = relative_path + "/right.png";
	faces.top = relative_path + "/up.png";
	faces.bottom = relative_path + "/down.png";
}

cube_textures Skybox::get_cube_faces()
{
	return faces;
}

glm::mat4 Skybox::getModelMatrix() {
	return model;
}

void Skybox::getCameraPosition(glm::vec3 pos) {
	cameraPosition = pos;
}
