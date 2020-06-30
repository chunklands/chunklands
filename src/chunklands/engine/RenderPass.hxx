#ifndef __CHUNKLANDS_ENGINE_RENDERPASS_HXX__
#define __CHUNKLANDS_ENGINE_RENDERPASS_HXX__

#include <chunklands/js.hxx>
#include <chunklands/modules/gl/gl_module.hxx>

namespace chunklands::engine {

  class RenderPass {
    JS_ATTR_WRAP(modules::gl::ProgramBase, Program)
  public:
    virtual ~RenderPass() {}

  public:
    virtual void Begin() {
      js_Program->Use();
    }

    virtual void End() {
      js_Program->Unuse();
    }

    virtual void UpdateBufferSize(int /*width*/, int /*height*/) {}
  
  protected:
    virtual void InitializeProgram() {}

    void JSCall_SetProgram(JSCbi info) {
      js_Program = info[0];

      js_Program->Use();
      InitializeProgram();
      js_Program->Unuse();
    }
  };

} // namespace chunklands::engine

#endif