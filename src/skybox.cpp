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
		meshes.push_back(mesh);
	}

	// Loading shaders
	return effect.load_from_file(shader_path("skybox.vs.glsl"), shader_path("skybox.fs.glsl"));
}

void Skybox::destroy()
{
	for (auto mesh : meshes) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
		glDeleteBuffers(1, &mesh.vao);
	}

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Skybox::update(float ms)
{
	// try to animate skybox later, i.e. make it rotate
}

void Skybox::draw(glm::mat4 mvp)
{
	// Setting shaders
	glUseProgram(effect.program);

	// Getting uniform locations for glUniform* calls
	GLuint mvp_uloc = glGetUniformLocation(effect.program, "mvp");
	
	for (auto mesh : meshes) {
		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		GLuint position_loc = glGetAttribLocation(effect.program, "position");
		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0); 
		glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);
		
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