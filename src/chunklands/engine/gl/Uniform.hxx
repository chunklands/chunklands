#ifndef __CHUNKLANDS_ENGINE_GL_UNIFORM_HXX__
#define __CHUNKLANDS_ENGINE_GL_UNIFORM_HXX__

#include "Program.hxx"
#include <chunklands/engine/gl/gl_check.hxx>
#include <chunklands/libcxx/glfw.hxx>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace chunklands::engine::gl {

inline void update_uniform(GLint location, GLint value)
{
    glUniform1i(location, value);
}

inline void update_uniform(GLint location, GLuint value)
{
    glUniform1ui(location, value);
}

inline void update_uniform(GLint location, GLfloat value)
{
    glUniform1f(location, value);
}

inline void update_uniform(GLint location, const glm::vec3& value)
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

inline void update_uniform(GLint location, const glm::mat4& value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

inline void update_uniform(GLint location, const glm::vec2& value)
{
    glUniform2fv(location, 1, glm::value_ptr(value));
}

template <class T>
class Uniform {
public:
    Uniform() {}
    Uniform(GLint location)
        : location_(location)
    {
        assert(location_ != -1);
    }

    Uniform(const Program& program, const std::string& name)
    {
        location_ = program.GetUniformLocation(name);
        assert(location_ != -1);
    }

    Uniform(const Program& program, const std::string& name, int index)
    {
        location_ = program.GetUniformLocation(name, index);
        assert(location_ != -1);
    }

    operator GLint() const
    {
        return location_;
    }

    inline void Update(const T& value)
    {
        assert(location_ != -1);
        update_uniform(location_, value);
        GL_CHECK_DEBUG();
    }

private:
    GLint location_ = -1;
};

} // namespace chunklands::gl

#endif