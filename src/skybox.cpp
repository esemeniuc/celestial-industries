#include "Skybox.hpp"

bool Skybox::init(OBJ::Data obj)
{
	// Clearing errors
	gl_flush_errors();

	GLuint vbo_id;
	// Vertex Buffer creation
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(vec3), obj.vertices.data(), GL_STATIC_DRAW);

	for (auto group : obj.groups) {
		Mesh mesh;
		mesh.vbo = vbo_id;
		// Index Buffer creation
		glGenBuffers(1, &mesh.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.vertexIndices.size() * sizeof(unsigned int), group.vertexIndices.data(), GL_STATIC_DRAW);

		// Vertex Array (Container for Vertex + Index buffer)
		glGenVertexArrays(1, &mesh.vao);
		if (gl_has_errors())
			return false;

		mesh.numIndices = group.vertexIndices.size();
		mesh.material = group.material;
		meshes.push_back(mesh);
	}

	// Loading shaders
	if (!effect.load_from_file(shader_path("skybox.vs.glsl"), shader_path("skybox.fs.glsl")))
		return false;

	return true;
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

bool Skybox::load_side_texture(GLuint texture, GLenum side, const char *texture_image_path)
{
	// load image data from file
	if (texture_image_path == nullptr) return false;
	int width, height, num_of_channels = 4;
	stbi_uc* texture_data = stbi_load(texture_image_path, &width, &height, NULL, num_of_channels);
	if (texture_data == NULL) return false;

	// bind texture to target
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	
	// assign image data to a target texture
	gl_flush_errors();
	glTexImage2D(side, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	// free memory
	free(texture_data);	
	return true;
}

void Skybox::generate_cube_map(const cube_faces * faces, GLuint *cube_texture)
{
	// generate a texture that will hold all cube map sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, cube_texture);

	// load textures for each side of the skybox cube
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, faces->front);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, faces->back);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, faces->top);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, faces->bottom);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, faces->left);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, faces->right);

	// cube map texture formatting (bilinear filtering, clamp to edge)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}