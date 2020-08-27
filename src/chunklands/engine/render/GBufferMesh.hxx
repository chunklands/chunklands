#ifndef __CHUNKLANDS_ENGINE_RENDER_GBUFFERMESH_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_GBUFFERMESH_HXX__

#include <chunklands/engine/gl/Vao.hxx>

namespace chunklands::engine::render {

struct GBufferMesh {
    gl::Vao<GL_TRIANGLES, CEVaoElementChunkBlock> vao;
};

} // namespace chunklands::engine::render

#endif