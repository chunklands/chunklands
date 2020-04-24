#include "gl.h"

#include <stdexcept>
#include "boost_stacktrace_import.h"
#include <sstream>

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