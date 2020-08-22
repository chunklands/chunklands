
#include "RenderQuad.hxx"

namespace chunklands::engine::gl {

  const gl::VaoElementRenderQuad quad_vertices[] = {
    {{-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f}},
    {{-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f}},
    {{ 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f}},
    {{ 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f}},
  };

  RenderQuad::RenderQuad() {
    vao_.Initialize(quad_vertices, 4);
  }

} // namespace chunklands::engine::gl