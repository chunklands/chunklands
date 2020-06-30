#ifndef __CHUNKLANDS_GL_H__
#define __CHUNKLANDS_GL_H__

#include <cassert>
#include "glfw.hxx"
#include <boost/preprocessor/stringize.hpp>
#include <chunklands/js.hxx>
#include <stb_image.h>
#include <glm/vec3.hpp>
#include <chunklands/modules/misc/misc_module.hxx>
#include <glm/gtc/type_ptr.hpp>


namespace chunklands::modules::gl {
  class ProgramBase : public JSObjectWrap<ProgramBase> {
    JS_IMPL_WRAP(ProgramBase, ONE_ARG({
      JS_CB(compile)
    }))

    JS_DECL_CB_VOID(compile)

  public:
    void Use() const {
      glUseProgram(program_);
    }

    void Unuse() const {
      glUseProgram(0);
    }

    GLint GetUniformLocation(const GLchar* name) const;

  private:
    GLuint program_ = 0;
  };

  class RenderQuad {
  public:
    RenderQuad();
    ~RenderQuad();

  public:
    void Render() {
      glBindVertexArray(vao_);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
    }

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
  };

  

  class Texture {
  public:
    ~Texture();

  public:
    void LoadTexture(const char *filename);
    void ActiveAndBind(GLenum texture) const;

    const glm::ivec2& GetSize() const {
      return size_;
    }
  
  private:
    GLuint texture_ = 0;
    glm::ivec2 size_;
  };

  class TextureLoader {
  public:
    TextureLoader(const std::string& filename);
    ~TextureLoader();

  public:
    stbi_uc* data = nullptr;
    GLenum format = GL_NONE;

    int width = 0;
    int height = 0;
  };

  std::ostream& operator<<(std::ostream& os, const TextureLoader& obj);

  class Uniform {
    friend const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform);

  public:
    Uniform(std::string name) : name_(std::move(name)) {}
    Uniform(const std::string& name, signed i) : name_(name + "[" + std::to_string(i) + "]") {}

    operator GLint() const {
      return location_;
    }

    void Update(GLint value) {
      CC_ASSERT(location_ != -1);
      glUniform1i(location_, value);
    }

    void Update(const glm::vec3& value) {
      CC_ASSERT(location_ != -1);
      glUniform3fv(location_, 1, glm::value_ptr(value));
    }

    void Update(const glm::mat4& value) {
      CC_ASSERT(location_ != -1);
      glUniformMatrix4fv(location_, 1, GL_FALSE, glm::value_ptr(value));
    }

    void FetchLocation(const ProgramBase& program) {
      location_ = program.GetUniformLocation(name_.c_str());
    }

  private:
    std::string name_;
    GLint location_ = -1;
  };

  const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform);
}

#endif