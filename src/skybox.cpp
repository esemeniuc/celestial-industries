#include "skybox.hpp"

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
	return effect.load_from_file(shader_path("skybox.vs.glsl"), shader_path("skybox.fs.glsl"))
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

bool Skybox::load_side_texture(GLuint texture, GLenum side, const std::string texture_image_path)
{
	// load image data from file
	if (texture_image_path.empty()) return false;
	int width, height, num_of_channels = 4;
	stbi_uc* texture_data = stbi_load(texture_image_path.c_str(), &width, &height, nullptr, num_of_channels);
	if (texture_data == nullptr) return false;

	// bind texture to target
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	
	// assign image data to a target texture
	gl_flush_errors();
	glTexImage2D(side, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	// free memory
	free(texture_data);	
	return !gl_has_errors();
}

void Skybox::generate_cube_map(const cube_textures &faces, GLuint *cube_texture)
{
	// assign texture to Texture unit 0
	glActiveTexture(GL_TEXTURE0);
	// generate texture name
	glGenTextures(1, cube_texture);

	// load textures for each side of the skybox cube
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, faces.front);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, faces.back);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, faces.top);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, faces.bottom);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, faces.left);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, faces.right);

	// cube map texture formatting (bilinear filtering, clamp to edge)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
