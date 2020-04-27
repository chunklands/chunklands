#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include "gl.h"
#include "js.h"
#include "WindowBase.h"
#include "WorldBase.h"
#include "GBufferPassBase.h"
#include "SSAOPassBase.h"
#include "SSAOBlurPassBase.h"
#include "LightingPassBase.h"
#include "SkyboxPassBase.h"
#include "SkyboxBase.h"
#include "RenderQuad.h"

namespace chunklands {
  class SceneBase : public JSObjectWrap<SceneBase> {
    JS_IMPL_WRAP(SceneBase, ONE_ARG({
      JS_SETTER(Window),
      JS_SETTER(World),
      JS_SETTER(GBufferPass),
      JS_SETTER(SSAOPass),
      JS_SETTER(SSAOBlurPass),
      JS_SETTER(LightingPass),
      JS_SETTER(SkyboxPass),
      JS_SETTER(Skybox),
    }))

    JS_DECL_SETTER_WRAP(WindowBase, Window)
    JS_DECL_SETTER_WRAP(WorldBase, World)
    JS_IMPL_SETTER_WRAP(GBufferPassBase, GBufferPass)
    JS_IMPL_SETTER_WRAP(SSAOPassBase, SSAOPass)
    JS_IMPL_SETTER_WRAP(SSAOBlurPassBase, SSAOBlurPass)
    JS_IMPL_SETTER_WRAP(LightingPassBase, LightingPass)
    JS_IMPL_SETTER_WRAP(SkyboxPassBase, SkyboxPass)
    JS_IMPL_SETTER_WRAP(SkyboxBase, Skybox)
  
  public:
    void Prepare();

    void Update(double diff);
    void Render(double diff);

    void UpdateViewport();
    void UpdateViewport(int width, int height);

  private:
    void InitializeGLBuffers(int width, int height);

  private:
    boost::signals2::scoped_connection window_on_resize_conn_;
    boost::signals2::scoped_connection window_on_cursor_move_conn_;
    glm::ivec2 last_cursor_pos_;

    glm::ivec2 buffer_size_;

    RenderQuad render_quad_;
  };
}

#endif