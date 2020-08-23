#ifndef __CHUNKLANDS_GBUFFERMESH_HXX__
#define __CHUNKLANDS_GBUFFERMESH_HXX__

#include "gl/Vao.hxx"

namespace chunklands::engine {

struct GBufferMesh {
    gl::Vao<GL_TRIANGLES, CEVaoElementChunkBlock> vao;
};

} // namespace chunklands::engine

#endif