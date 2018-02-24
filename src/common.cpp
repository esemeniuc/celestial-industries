#include "common.hpp"
#include "logger.hpp"

// stlib
#include <vector>
#include <sstream>
#include <string>
#include <iostream>

// Our stuff

char separator()
{
	return '/';
}

std::vector<std::string> splitString(std::string input, const char seperator) {
	std::vector<std::string> result;
	std::string segment;
	std::stringstream inputStream;
	inputStream << input;
	while (std::getline(inputStream, segment, seperator)) {
		result.push_back(segment);
	}
	return result;
}

std::string pathBuilder(std::vector<std::string> parts) {
	std::stringstream path;

	// Because the template has the vs files in their own folder the exe expects stuff to be relative to the INSIDE of that folder :)
#ifdef _WIN32
		path << "../";
#endif
	for (auto part : parts) {
		path << part << separator();
	}
	return path.str();
}

std::string pathAppender(std::string base, std::vector<std::string> parts)
{
	std::stringstream path;
	path << base;
	bool first = true;
	for (auto part : parts) {
		if (!first) {
			path << separator();
		}
		else {
			first = false;
		}
		path << part;
	}
	return path.str();
}

void gl_flush_errors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
	}

	return true;
}

namespace
{
	bool gl_compile_shader(GLuint shader)
	{
		glCompileShader(shader);
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetShaderInfoLog(shader, log_len, &log_len, log.data());
			glDeleteShader(shader);

			fprintf(stderr, "GLSL: %s", log.data());
			return false;
		}

		return true;
	}
}

bool Shader::load_from_file(const char* vs_path, const char* fs_path)
{
	gl_flush_errors();

	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);

	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	// Compiling
	// Shaders already delete if compilation fails
	if (!gl_compile_shader(vertex))
		return false;

	if (!gl_compile_shader(fragment))
	{
		glDeleteShader(vertex);
		return false;
	}

	// Linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	{
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(program, log_len, &log_len, log.data());

			release();
			fprintf(stderr, "Link error: %s", log.data());
			return false;
		}
	}

	if (gl_has_errors())
	{
		release();
		fprintf(stderr, "OpenGL errors occured while compiling Shader");
		return false;
	}

	return true;
}

void Shader::release()
{
	glDeleteProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

std::pair<bool, std::shared_ptr<std::vector<Mesh>>> objToMesh(OBJ::Data obj)
{
    logger(LogLevel::DEBUG) << "Converting to meshes... " << Logger::endl;
    // Clearing errors
    gl_flush_errors();

    GLuint vbo_id;
    // Vertex Buffer creation
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, obj.data.size() * sizeof(OBJ::VertexData), obj.data.data(), GL_STATIC_DRAW);

    auto meshes = std::make_shared<std::vector<Mesh>>();
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
            return { false, meshes };

        mesh.numIndices = group.indices.size();
        mesh.material = group.material;
        meshes->push_back(mesh);
    }

    return { true, meshes };
}