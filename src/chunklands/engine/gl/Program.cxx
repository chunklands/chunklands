
#include "Program.hxx"
#include "gl_check.hxx"
#include "gl_exception.hxx"
#include <vector>

namespace chunklands::engine::gl {

GLuint compile(const GLuint shader, const GLchar* const source)
{
    GL_CHECK_DEBUG();

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    GL_CHECK_DEBUG();

    if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> message(length + 1);
        glGetShaderInfoLog(shader, length, nullptr, message.data());
        glDeleteShader(shader);

        throw_gl_exception("glCompileShader", message.data());
    }

    return shader;
}

Program::Program(const char* const vsh_source, const char* const fsh_source)
{
    GL_CHECK_DEBUG();

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    compile(vertex_shader, vsh_source);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile(fragment_shader, fsh_source);

    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);
    GL_CHECK_DEBUG();

    GLint result = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> message(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, message.data());
        throw_gl_exception("glLinkProgram", message.data());
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

GLint Program::GetUniformLocation(const std::string& name) const
{
    const GLint location = glGetUniformLocation(program_, name.data());
    if (location == -1) {
        std::string message = "illegal uniform location: " + name;
        throw_gl_exception("glGetUniformLocation", std::move(message));
    }

    return location;
}

GLint Program::GetUniformLocation(const std::string& name, const int index) const
{
    return GetUniformLocation(name + "[" + std::to_string(index) + "]");
}

} // namespace chunklands::gl