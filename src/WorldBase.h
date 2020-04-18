#ifndef __CHUNKLANDS_WORLDBASE_H__
#define __CHUNKLANDS_WORLDBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <boost/functional/hash.hpp>
#include <napi.h>
#include <queue>
#include <unordered_map>
#include <vector>
#include "Chunk.h"
#include "ChunkGeneratorBase.h"
#include "gl.h"
#include "napi/object_wrap_util.h"
#include "napi/PersistentObjectWrap.h"
#include "GLProgramBase.h"
#include "RenderQuad.h"

namespace chunklands {

  class WorldBase : public Napi::ObjectWrap<WorldBase> {
    DECLARE_OBJECT_WRAP(WorldBase)
    DECLARE_OBJECT_WRAP_CB(void SetChunkGenerator)
    DECLARE_OBJECT_WRAP_CB(void SetGBufferShader)
    DECLARE_OBJECT_WRAP_CB(void SetLightingShader)

  private:
    struct ivec3_hasher {
      std::size_t operator()(const glm::ivec3& v) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, boost::hash_value(v.x));
        boost::hash_combine(seed, boost::hash_value(v.y));
        boost::hash_combine(seed, boost::hash_value(v.z));

        return seed;
      }
    };

  public:
    void Prepare();
    void Update(double diff);
    void RenderGBufferPass(double diff);
    void RenderDeferredLightingPass(double diff, GLuint position_texture, GLuint normal_texture, GLuint color_texture);

    void UpdateViewportRatio(int width, int height);

    void AddLook(float yaw_rad, float pitch_rad);
    void AddPos(const glm::vec3& v);

    const glm::vec2& GetLook() const {
      return look_;
    }

  private:

    NapiExt::PersistentObjectWrap<ChunkGeneratorBase> chunk_generator_;
    NapiExt::PersistentObjectWrap<GLProgramBase> g_buffer_shader_;
    NapiExt::PersistentObjectWrap<GLProgramBase> lighting_shader_;

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ivec3_hasher> chunk_map_;

    glm::vec3 pos_ = glm::vec3(8.f, 0.7f, 60.f);
    glm::vec2 look_;

    glm::mat4 view_;
    GLint view_uniform_location_ = -1;

    glm::mat4 proj_;
    GLint proj_uniform_location_ = -1;

    GLint texture_location_ = -1;
    GLint position_location_ = -1;
    GLint normal_location_ = -1;
    GLint color_location_ = -1;

    GLint render_distance_location_ = -1;

    std::vector<glm::ivec3> nearest_chunks_;

    std::unique_ptr<RenderQuad> render_quad_;
  };
}



#endif
