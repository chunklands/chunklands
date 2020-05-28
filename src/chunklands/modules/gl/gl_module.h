#ifndef __CHUNKLANDS_GL_H__
#define __CHUNKLANDS_GL_H__

#include <cassert>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <boost/preprocessor/stringize.hpp>
#include <chunklands/js.h>
#include <stb_image.h>
#include <glm/vec3.hpp>
#include <chunklands/modules/misc/misc_module.h>
#include <glm/gtc/type_ptr.hpp>

#define POSITION __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__)

#ifdef NDEBUG

#define CHECK_GL() \
  do {} while (0)

#define CHECK_GL_HERE() \
  do {} while (0)

#else // NDEBUG

#define CHECK_GL() \
  ::chunklands::GLCheck gl_check(POSITION);

#define CHECK_GL_HERE() \
  gl_check.Check(POSITION)
#endif // NDEBUG

namespace chunklands {
  class GLCheck {
  public:
    GLCheck(const char* hint);
    ~GLCheck();

  public:
    void Check(const char* hint);
  };
}

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
    void ActiveAndBind(GLenum texture);
  
  private:
    GLuint texture_ = 0;
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