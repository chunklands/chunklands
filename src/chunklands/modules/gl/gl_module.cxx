#include "gl_module.hxx"

#include <sstream>
#include <stdexcept>

#include <chunklands/modules/misc/misc_module.hxx>

namespace chunklands {
  GLCheck::GLCheck(const char* hint) {
    Check(hint);
  }

  GLCheck::~GLCheck() {
    Check("end of scope");
  }

  void GLCheck::Check(const char* hint) {
    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
      return;
    }

    std::stringstream ss;
    ss << "GL ERROR (";

    switch (err)
    {
      case GL_INVALID_ENUM:                  ss << "INVALID_ENUM"; break;
      case GL_INVALID_VALUE:                 ss << "INVALID_VALUE"; break;
      case GL_INVALID_OPERATION:             ss << "INVALID_OPERATION"; break;
      case GL_STACK_OVERFLOW:                ss << "STACK_OVERFLOW"; break;
      case GL_STACK_UNDERFLOW:               ss << "STACK_UNDERFLOW"; break;
      case GL_OUT_OF_MEMORY:                 ss << "OUT_OF_MEMORY"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: ss << "INVALID_FRAMEBUFFER_OPERATION"; break;
    }
    
    ss << ") ";  
    ss << hint;
    ss << "\n\n";
    ss << boost::stacktrace::stacktrace();
    
    throw std::runtime_error(ss.str());
  }
}

namespace chunklands::modules::gl {

  /////////////////////////////////////////////////////////////////////////////
  // ProgramBase //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(ProgramBase)

  void ProgramBase::JSCall_compile(JSCbi info) {
    std::string vsh_src = info[0].ToString(),
                fsh_src = info[1].ToString();

    GLuint vsh, fsh;

    { // vertex shader
      CHECK_GL();

      vsh = glCreateShader(GL_VERTEX_SHADER);
      const char* src = vsh_src.c_str();
      glShaderSource(vsh, 1, &src, nullptr);
      glCompileShader(vsh);
      GLint result = GL_FALSE;
      glGetShaderiv(vsh, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(vsh, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(vsh, length, nullptr, message.data());
        throw JSError::New(Env(), message.data());
      }
    }

    { // fragment shader
      CHECK_GL();

      fsh = glCreateShader(GL_FRAGMENT_SHADER);
      const char* src = fsh_src.c_str();
      glShaderSource(fsh, 1, &src, nullptr);
      glCompileShader(fsh);
      GLint result = GL_FALSE;
      glGetShaderiv(fsh, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(fsh, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(fsh, length, nullptr, message.data());
        throw JSError::New(Env(), message.data());
      }
    }

    { // program
      CHECK_GL();

      program_ = glCreateProgram();
      glAttachShader(program_, vsh);
      glAttachShader(program_, fsh);
      glLinkProgram(program_);

      GLint result = GL_FALSE;
      glGetProgramiv(program_, GL_LINK_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, message.data());
        throw JSError::New(Env(), message.data());
      }

      glDeleteShader(vsh);
      vsh = 0;
      glDeleteShader(fsh);
      fsh = 0;
    }
  }

  GLint ProgramBase::GetUniformLocation(const GLchar* name) const {
    GLint location = glGetUniformLocation(program_, name);
    CHECK_MSG(location != -1, std::string("for ") + name);

    return location;
  }



  /////////////////////////////////////////////////////////////////////////////
  // RenderQuad ///////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  const GLfloat quad_vertices[] = {
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
  };

  RenderQuad::RenderQuad() {
    CHECK_GL();
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
  }

  RenderQuad::~RenderQuad() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // Texture //////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Texture::~Texture() {
    if (texture_ != 0) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  void Texture::LoadTexture(const char* filename) {
    CHECK_GL();
    TextureLoader loader(filename);

    // TODO(daaitch): load POT dimension texture
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size_.x = loader.width;
    size_.y = loader.height;
  }
  
  void Texture::ActiveAndBind(GLenum texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, texture_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // TextureLoader ////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  TextureLoader::TextureLoader(const std::string& filename) {
    int comp;
    data = stbi_load(filename.c_str(), &width, &height, &comp, 0);
    
    if (!data) {
      throw std::runtime_error(std::string("could not load ") + filename);
    }

    switch (comp) {
    case 3: {
      format = GL_RGB;
      break;
    }
    case 4: {
      format = GL_RGBA;
      break;
    }
    default: {
      throw std::runtime_error(std::string("only support 3 or 4 channels"));
    }
    }
  }

  TextureLoader::~TextureLoader() {
    if (data != nullptr) {
      stbi_image_free(data);
      data = nullptr;
    }
  }

  std::ostream& operator<<(std::ostream& os, const TextureLoader& obj) {
    return os
      << "TextureLoader {\n"
      << "  data   = " << (void *)obj.data << "\n"
      << "  format = " << obj.format << "\n"
      << "  width  = " << obj.width << "\n"
      << "  height = " << obj.height << "\n"
      << "}"
      << std::endl;
  }



  /////////////////////////////////////////////////////////////////////////////
  // Uniform //////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform) {
    uniform.location_ = program.GetUniformLocation(uniform.name_.c_str());
    return program;
  }
}