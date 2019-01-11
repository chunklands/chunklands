#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include <boost/functional/hash.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <napi.h>
#include <unordered_map>
#include "Chunk.h"
#include "gl.h"
#include "napi/UnwrappedObject.h"
#include "WindowBase.h"

namespace chunklands {
  class SceneBase : public Napi::ObjectWrap<SceneBase> {
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
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public:
    SceneBase(const Napi::CallbackInfo& info);

  public: // JS
    void SetWindow(const Napi::CallbackInfo& info);
  
  public: // Native
    void Prepare();
    void Render(double diff);

    void UpdateViewport(int width, int height);

  private:
    NapiExt::PersistentObjectWrap<WindowBase> window_;
    boost::signals2::scoped_connection window_on_resize_conn_;

    std::string vsh_src_;
    std::string fsh_src_;
    
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ivec3_hasher> chunk_map_;

    GLuint vsh_;
    GLuint fsh_;
    GLuint program_;

    glm::vec3 pos_ = glm::vec3(8.f, 16.7f, 60.f);

    glm::mat4 view_;
    GLint view_uniform_location_;

    glm::mat4 proj_;
    GLint proj_uniform_location_;
  };
}

#endif