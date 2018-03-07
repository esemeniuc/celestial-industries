#include "shader.hpp"

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

            logger(LogLevel::ERR) << "GLSL: " << log.data() << '\n';

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
        logger(LogLevel::ERR) << "Failed to load shader files: " << vs_path << fs_path << '\n';
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
            logger(LogLevel::ERR) << "Link error: " << log.data() << '\n';
            return false;
        }
    }

    if (gl_has_errors())
    {
        release();
        logger(LogLevel::ERR) << "OpenGL errors occured while compiling Shader" << '\n';
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
