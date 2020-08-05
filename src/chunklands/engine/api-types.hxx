#ifndef __CHUNKLANDS_ENGINE_API_TYPES_HXX__
#define __CHUNKLANDS_ENGINE_API_TYPES_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <string>
#include <vector>

namespace chunklands::engine {

  struct CEHandle;
  struct CEWindowHandle;
  struct CEChunkHandle;
  struct CEGBufferMeshHandle;
  struct CEBlockRegistrar;
  struct CEBlockHandle;

  constexpr int CE_CHUNK_SIZE = 32;
  constexpr int CE_CHUNK_BLOCK_COUNT = CE_CHUNK_SIZE * CE_CHUNK_SIZE * CE_CHUNK_SIZE;
  constexpr int CE_CHUNK_STATE_COUNT = 3;

  struct __attribute__ ((packed)) CEVaoElementChunkBlock {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
  };

  struct CEBlockCreateInit {
    std::string id;
    bool opaque = false;
    std::vector<CEVaoElementChunkBlock> data;
    std::vector<unsigned char> texture;
  };

  struct CEPassInit {
    std::string vertex_shader;
    std::string fragment_shader;
  };

  struct CERenderPipelineInit {
    CEPassInit gbuffer;
    CEPassInit lighting;
  };

} // namespace chunklands::engine

#endif