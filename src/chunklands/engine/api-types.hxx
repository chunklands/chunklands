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

  enum FaceType {
    kFaceTypeUnknown,
    kFaceTypeLeft,
    kFaceTypeRight,
    kFaceTypeTop,
    kFaceTypeBottom,
    kFaceTypeFront,
    kFaceTypeBack,
  };

  struct __attribute__ ((packed)) CEVaoElementChunkBlock {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
  };

  struct CEBlockFace {
    FaceType type;
    std::vector<CEVaoElementChunkBlock> data;
  };

  struct CEBlockCreateInit {
    std::string id;
    bool opaque = false;
    std::vector<CEBlockFace> faces;
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

  struct CEEvent {
    CEEvent(std::string type) : type(std::move(type)) {}
    std::string type;
  };

  struct CEWindowEvent : public CEEvent {
    using CEEvent::CEEvent;

    union {
      struct {
        int button;
        int action;
        int mods;
      } click;

      struct {
        int key;
        int scancode;
        int action;
        int mods;
      } key;
    };
  };

  struct CECameraPosition {
    float x, y, z;
  };

  struct CECameraEvent : public CEEvent {
    using CEEvent::CEEvent;
    union {
      CECameraPosition positionchange;
    };
  };

} // namespace chunklands::engine

#endif