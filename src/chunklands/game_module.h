#ifndef __CHUNKLANDS_GAME_H__
#define __CHUNKLANDS_GAME_H__

#include <chunklands/js.h>
#include "gl_module.h"
#include "engine_module.h"
#include <glm/vec3.hpp>
#include <queue>
#include <boost/functional/hash.hpp>

namespace chunklands::game {

  class BlockDefinition {
  public:
    BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data);

  public:
    const std::string& GetId() const {
      return id_;
    }

    bool IsOpaque() const {
      return opaque_;
    }

    const std::unordered_map<std::string, std::vector<GLfloat>>& GetFacesVertexData() const {
      return faces_vertex_data_;
    }

  private:
    std::string id_;
    bool opaque_ = false;
    std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data_;
  };

  class BlockRegistrarBase : public JSObjectWrap<BlockRegistrarBase> {
    JS_IMPL_WRAP(BlockRegistrarBase, ONE_ARG({
      JS_CB(addBlock),
      JS_CB(loadTexture),
      JS_CB(getBlockIds)
    }))

    JS_DECL_CB_VOID(addBlock)
    JS_DECL_CB_VOID(loadTexture)
    JS_DECL_CB(getBlockIds)

  public:
    BlockDefinition* GetByIndex(int index);
    void BindTexture();

  private:
    std::vector<std::unique_ptr<BlockDefinition>> block_definitions_;

    gl::Texture texture_;
  };

  enum ChunkState {
    kEmpty,
    kModelIsPreparing,
    kModelPrepared,
    kViewPrepared
  };

  enum ChunkNeighbor {
    kLeft,   // -x
    kRight,  // +x
    kTop,    // +y
    kBottom, // -y
    kFront,  // +z
    kBack,   // -z

    kNeighborCount = 6
  };
  class ChunkGenerator;
  
  class Chunk {
    friend ChunkGenerator;

  public:
    static constexpr unsigned SIZE_LB = 4;
    static constexpr unsigned SIZE = 1 << SIZE_LB;
    static constexpr unsigned BLOCK_COUNT = SIZE * SIZE * SIZE;

  public:
    using BlockType = BlockDefinition*;
    using BlocksType = std::array<std::array<std::array<BlockType, SIZE>, SIZE>, SIZE>;
  
  public:
    Chunk(glm::ivec3 pos);
    ~Chunk();
  
  public:
    void InitializeGL();
    void Cleanup();
    void Render();

    template <typename CbFn>
    void ForEachBlock(const CbFn& fn);

    GLuint GetIndexCount() const {
      return vb_index_count_;
    }

    ChunkState GetState() const {
      return state_;
    }

    const glm::ivec3& GetPos() const {
      return pos_;
    }

  private:
    ChunkState state_ = kEmpty;
    glm::ivec3 pos_;
    BlocksType blocks_;

    GLuint vb_index_count_ = 0;
    GLuint vao_ = 0;
    GLuint vb_ = 0;
  };

  template <typename CbFn>
  void Chunk::ForEachBlock(const CbFn& fn) {
    for (unsigned z = 0; z < SIZE; z++) {
      auto&& items_yx = blocks_[z];
      for (unsigned y = 0; y < SIZE; y++) {
        auto&& items_x = items_yx[y];
        for (unsigned x = 0; x < SIZE; x++) {
          auto&& item = items_x[x];
          fn(item, x, y, z);
        }
      }
    }
  }

  namespace detail {
    struct loaded_chunks_data {
      std::shared_ptr<Chunk> chunk;
      JSObjectRef js_chunk_data;
    };
  }

  class ChunkGenerator : public JSObjectWrap<ChunkGenerator> {
    JS_IMPL_WRAP(ChunkGenerator, ONE_ARG({
      JS_SETTER(BlockRegistrar),
      JS_SETTER(WorldGenerator),
    }))

    JS_IMPL_SETTER_WRAP(BlockRegistrarBase, BlockRegistrar)
    JS_IMPL_SETTER_OBJECT(WorldGenerator)

  public:
    void GenerateModel(std::shared_ptr<Chunk>& chunk);
    bool ProcessNextModel();
    void GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]);

    void BindTexture();

  private:

    std::queue<detail::loaded_chunks_data> loaded_chunks_;
  };

  class World : public JSObjectWrap<World> {
    JS_IMPL_WRAP(World, ONE_ARG({
      JS_SETTER(ChunkGenerator),
    }))

    JS_IMPL_SETTER_WRAP(ChunkGenerator, ChunkGenerator)

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
    void RenderChunks(double diff);

    void UpdateViewportRatio(int width, int height);

    void AddLook(float yaw_rad, float pitch_rad);
    void AddPos(const glm::vec3& v) {
      pos_ += v;
    }

    const glm::vec2& GetLook() const {
      return look_;
    }

    const glm::mat4& GetProjection() const {
      return proj_;
    }

    const glm::mat4& GetView() const {
      return view_;
    }

    const glm::mat4& GetViewSkybox() const {
      return view_skybox_;
    }

    int GetRenderDistance() const;

  private:

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ivec3_hasher> chunk_map_;

    glm::vec3 pos_ = glm::vec3(8.f, 0.7f, 60.f);
    glm::vec2 look_;

    glm::mat4 view_;
    glm::mat4 proj_;

    glm::mat4 view_skybox_;

    std::vector<glm::ivec3> nearest_chunks_;
  };

  class Scene : public JSObjectWrap<Scene>, public engine::IScene {
    JS_IMPL_WRAP(Scene, ONE_ARG({
      JS_SETTER(Window),
      JS_SETTER(World),
      JS_SETTER(GBufferPass),
      JS_SETTER(SSAOPass),
      JS_SETTER(SSAOBlurPass),
      JS_SETTER(LightingPass),
      JS_SETTER(SkyboxPass),
      JS_SETTER(Skybox),
      InstanceMethod("asIScene", &Scene::x)
    }))

    JSValue x(JSCbi) {
      return js_abstract_wrap<IScene>(this);
    }

    JS_DECL_SETTER_WRAP(engine::Window, Window)
    JS_DECL_SETTER_WRAP(World, World)
    JS_IMPL_SETTER_WRAP(engine::GBufferPass, GBufferPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOPass, SSAOPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOBlurPass, SSAOBlurPass)
    JS_IMPL_SETTER_WRAP(engine::LightingPass, LightingPass)
    JS_IMPL_SETTER_WRAP(engine::SkyboxPass, SkyboxPass)
    JS_IMPL_SETTER_WRAP(engine::Skybox, Skybox)
  
  public:
    virtual void Prepare() override;

    virtual void Update(double diff) override;
    virtual void Render(double diff) override;

    void UpdateViewport();
    void UpdateViewport(int width, int height);

  private:
    void InitializeGLBuffers(int width, int height);

  private:
    boost::signals2::scoped_connection window_on_resize_conn_;
    boost::signals2::scoped_connection window_on_cursor_move_conn_;
    glm::ivec2 last_cursor_pos_;

    glm::ivec2 buffer_size_;

    gl::RenderQuad render_quad_;
  };
}

#endif
