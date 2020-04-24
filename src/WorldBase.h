#ifndef __CHUNKLANDS_WORLDBASE_H__
#define __CHUNKLANDS_WORLDBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <boost/functional/hash.hpp>
#include <queue>
#include <unordered_map>
#include <vector>
#include "Chunk.h"
#include "ChunkGeneratorBase.h"
#include "gl.h"
#include "napi/object_wrap_util.h"
#include "GLProgramBase.h"
#include "RenderQuad.h"
#include "SkyboxBase.h"
#include "GBufferPass.h"
#include "SSAOPass.h"
#include "SSAOBlurPass.h"
#include "LightingPass.h"
#include "js.h"
#include "SkyboxPassBase.h"

namespace chunklands {

  class WorldBase : public JSWrap<WorldBase> {
    JS_DECL_WRAP(WorldBase)
    JS_DECL_SETTER_REF(ChunkGeneratorBase, ChunkGenerator)
    DECLARE_OBJECT_WRAP_CB(void SetGBufferShader)
    DECLARE_OBJECT_WRAP_CB(void SetSSAOShader)
    DECLARE_OBJECT_WRAP_CB(void SetSSAOBlurShader)
    DECLARE_OBJECT_WRAP_CB(void SetLightingShader)
    DECLARE_OBJECT_WRAP_CB(void SetSkyboxShader)
    JS_DECL_SETTER_REF(SkyboxBase, Skybox)
    JS_DECL_SETTER_REF(SkyboxPassBase, SkyboxPass)

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
    void RenderSSAOPass(double diff, GLuint position_texture, GLuint normal_texture, GLuint noise_texture);
    void RenderSSAOBlurPass(double diff, GLuint ssao_texture);
    void RenderDeferredLightingPass(double diff, GLuint position_texture, GLuint normal_texture, GLuint color_texture, GLuint ssao_texture);
    void RenderSkybox(double diff);

    void UpdateViewportRatio(int width, int height);

    void AddLook(float yaw_rad, float pitch_rad);
    void AddPos(const glm::vec3& v);

    const glm::vec2& GetLook() const {
      return look_;
    }  

  private:

    GBufferPass  g_buffer_pass;
    SSAOPass     ssao_pass;
    SSAOBlurPass ssao_blur_pass;
    LightingPass lighting_pass_;

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ivec3_hasher> chunk_map_;

    glm::vec3 pos_ = glm::vec3(8.f, 0.7f, 60.f);
    glm::vec2 look_;

    glm::mat4 view_;
    glm::mat4 proj_;

    glm::mat4 view_skybox_;

    std::vector<glm::ivec3> nearest_chunks_;

    std::unique_ptr<RenderQuad> render_quad_;
  };
}

#endif
