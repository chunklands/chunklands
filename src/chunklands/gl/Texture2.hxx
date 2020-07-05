#ifndef __CHUNKLANDS_GL_TEXTURE2_HXX__
#define __CHUNKLANDS_GL_TEXTURE2_HXX__

#include <chunklands/js.hxx>
#include <chunklands/gl/glfw.hxx>
#include <glm/vec2.hpp>

namespace chunklands::gl {

class Texture2 : public JSObjectWrap<Texture2> {
  JS_IMPL_WRAP(Texture2, ONE_ARG({
    JS_CB(load)
  }))

  JS_DECL_CB_VOID(load)
public:
  ~Texture2();

  void ActiveAndBind(GLenum active_texture) const;

private:
  GLuint texture_ = 0;
  glm::ivec2 size_;
};

} // namespace chunklands::gl

#endif