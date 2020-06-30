#ifndef __CHUNKLANDS_GAME_H__
#define __CHUNKLANDS_GAME_H__

#include <chunklands/debug.hxx>
#include <chunklands/js.hxx>
#include <chunklands/modules/gl/gl_module.hxx>
#include <chunklands/modules/engine/IScene.hxx>
#include <chunklands/modules/engine/ICollisionSystem.hxx>
#include <chunklands/modules/engine/Camera.hxx>
#include <chunklands/modules/engine/Window.hxx>
#include <chunklands/modules/engine/GBufferPass.hxx>
#include <chunklands/modules/engine/SSAOPass.hxx>
#include <chunklands/modules/engine/SSAOBlurPass.hxx>
#include <chunklands/modules/engine/LightingPass.hxx>
#include <chunklands/modules/engine/SkyboxPass.hxx>
#include <chunklands/modules/engine/TextRenderer.hxx>
#include <chunklands/modules/engine/GameOverlayRenderer.hxx>
#include <chunklands/modules/engine/BlockSelectPass.hxx>
#include <chunklands/modules/engine/Skybox.hxx>
#include <chunklands/modules/engine/Camera.hxx>
#include <chunklands/modules/engine/IScene.hxx>
#include <chunklands/modules/engine/MovementController.hxx>
#include <chunklands/math.hxx>
#include <glm/vec3.hpp>
#include <queue>
#include <unordered_map>

namespace chunklands::modules::game {

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

    engine::collision_result ProcessCollision(const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement) const;

  private:
    std::string id_;
    bool opaque_ = false;
    std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data_;
  };

  class BlockRegistrar : public JSObjectWrap<BlockRegistrar> {
    JS_IMPL_WRAP(BlockRegistrar, ONE_ARG({
      JS_CB(addBlock),
      JS_CB(loadTexture)
    }))

    JS_DECL_CB(addBlock)
    JS_DECL_CB_VOID(loadTexture)

  public:
    BlockDefinition* GetByIndex(int index);
    const BlockDefinition* GetByIndex(int index) const;
    void BindTexture();

  private:
    std::vector<std::unique_ptr<BlockDefinition>> block_definitions_;

    gl::Texture texture_;
  };

  struct sprite_definition {
    std::string id;
    std::unordered_map<std::string, std::vector<GLfloat>> faces;
  };

  class SpriteRegistrar : public JSObjectWrap<SpriteRegistrar> {
    JS_IMPL_WRAP(SpriteRegistrar, ONE_ARG({
      JS_CB(addSprite),
    }))

    JS_DECL_CB_VOID(addSprite)

  public:
    const sprite_definition* GetSprite(const std::string& id) const;

  private:
    std::unordered_map<std::string, std::unique_ptr<sprite_definition>> sprites_;
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

    inline const BlockDefinition* BlockAt(const glm::uvec3& at) const {
      assert(at.x < SIZE && at.y < SIZE && at.z < SIZE);
      return blocks_[at.z][at.y][at.x];
    }

    inline const BlockDefinition* BlockAt(const glm::ivec3& at) const {
      assert(at.x >= 0 && at.y >= 0 && at.z >= 0);
      assert(at.x < (int)SIZE && at.y < (int)SIZE && at.z < (int)SIZE);
      
      return blocks_[at.z][at.y][at.x];
    }

    void UpdateBlock(const glm::ivec3& at, BlockDefinition* block_def) {
      assert(block_def);

      assert(at.x >= 0 && at.y >= 0 && at.z >= 0);
      assert(at.x < (int)SIZE && at.y < (int)SIZE && at.z < (int)SIZE);

      blocks_[at.z][at.y][at.x] = block_def;
      state_ = ChunkState::kModelPrepared;
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
      JSRef<JSInt32Array> js_chunk_data;
    };
  }

  class ChunkGenerator : public JSObjectWrap<ChunkGenerator> {
    JS_IMPL_WRAP(ChunkGenerator, ONE_ARG({
      JS_SETTER(BlockRegistrar),
      JS_SETTER(WorldGenerator),
    }))

    JS_IMPL_SETTER_WRAP(BlockRegistrar, BlockRegistrar)
    JS_IMPL_SETTER_OBJECT(WorldGenerator)

  public:
    void GenerateModel(std::shared_ptr<Chunk>& chunk);
    bool ProcessNextModel();
    void GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]);

    void BindTexture();

    const BlockRegistrar& GetBlockRegistrar() const {
      return *js_BlockRegistrar.Get();
    }

    BlockRegistrar& GetBlockRegistrar() {
      return *js_BlockRegistrar.Get();
    }

  private:

    std::queue<detail::loaded_chunks_data> loaded_chunks_;
  };

  struct block_collision {
    engine::collision_result collision_result;
    const BlockDefinition* block_def;
    math::ivec3 pos_in_chunk;
    Chunk *chunk;
  };

  class World : public JSObjectWrap<World>, public engine::ICollisionSystem {
    JS_IMPL_WRAP(World, ONE_ARG({
      JS_SETTER(ChunkGenerator),
      JS_SETTER(BlockRegistrar),
      JS_ABSTRACT_WRAP(engine::ICollisionSystem, ICollisionSystem),
      JS_CB(findPointingBlock),
      JS_CB(replaceBlock),
    }))

    JS_IMPL_SETTER_WRAP(ChunkGenerator, ChunkGenerator)
    JS_IMPL_SETTER_WRAP(BlockRegistrar, BlockRegistrar)
    JS_IMPL_ABSTRACT_WRAP(engine::ICollisionSystem, ICollisionSystem)
    JS_DECL_CB(findPointingBlock)
    JS_DECL_CB_VOID(replaceBlock)

  public:
    engine::collision_result ProcessNextCollision(const math::fAABB3 &box, const math::fvec3 &movement) override;
    std::optional<math::ivec3> FindPointingBlock(const math::fLine3& look);

    const BlockDefinition* GetBlock(const glm::ivec3& coord) const;

  public:
    void Prepare();
    void Update(double diff, const engine::Camera& camera);
    void Render(double diff, const engine::Camera& camera);

    int GetRenderDistance() const;

  private:

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, math::ivec3_hasher> chunk_map_;

    std::vector<glm::ivec3> nearest_chunks_;
  };

  class GameOverlay : public JSObjectWrap<GameOverlay> {
    JS_IMPL_WRAP(GameOverlay, ONE_ARG({
      JS_SETTER(SpriteRegistrar),
    }))

    JS_IMPL_SETTER_WRAP(SpriteRegistrar, SpriteRegistrar)

  public:
    void Prepare();
    void Update(double diff);
    void Render(double diff);

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;

    GLsizei vb_index_count_ = 0;
  };

  class Scene : public JSObjectWrap<Scene>, public engine::IScene {
    JS_IMPL_WRAP(Scene, ONE_ARG({
      JS_SETTER(Window),
      JS_SETTER(World),
      JS_SETTER(BlockRegistrar), // TODO(daaitch): dirty, only for the texture
      JS_SETTER(GBufferPass),
      JS_SETTER(SSAOPass),
      JS_SETTER(SSAOBlurPass),
      JS_SETTER(LightingPass),
      JS_SETTER(SkyboxPass),
      JS_SETTER(TextRenderer),
      JS_SETTER(GameOverlayRenderer),
      JS_SETTER(BlockSelectPass),
      JS_SETTER(Skybox),
      JS_SETTER(Camera),
      JS_ABSTRACT_WRAP(engine::IScene, IScene),
      JS_SETTER(MovementController),
      JS_SETTER(GameOverlay),
      JS_SETTER(FlightMode),
      JS_GETTER(FlightMode),
    }))

    JS_DECL_SETTER_WRAP(engine::Window, Window)
    JS_DECL_SETTER_WRAP(World, World)
    JS_IMPL_SETTER_WRAP(BlockRegistrar, BlockRegistrar)
    JS_IMPL_SETTER_WRAP(engine::GBufferPass, GBufferPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOPass, SSAOPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOBlurPass, SSAOBlurPass)
    JS_IMPL_SETTER_WRAP(engine::LightingPass, LightingPass)
    JS_IMPL_SETTER_WRAP(engine::SkyboxPass, SkyboxPass)
    JS_IMPL_SETTER_WRAP(engine::TextRenderer, TextRenderer)
    JS_IMPL_SETTER_WRAP(engine::GameOverlayRenderer, GameOverlayRenderer)
    JS_IMPL_SETTER_WRAP(engine::BlockSelectPass, BlockSelectPass)
    JS_IMPL_SETTER_WRAP(engine::Skybox, Skybox)
    JS_IMPL_SETTER_WRAP(engine::Camera, Camera)
    JS_IMPL_ABSTRACT_WRAP(engine::IScene, IScene)
    JS_IMPL_SETTER_WRAP(engine::MovementController, MovementController)
    JS_IMPL_SETTER_WRAP(GameOverlay, GameOverlay)
    
    JSValue JSCall_GetFlightMode(JSCbi info) {
      return JSBoolean::New(info.Env(), flight_mode_);
    }

    void JSCall_SetFlightMode(JSCbi info) {
      flight_mode_ = info[0].ToBoolean();
    }
  
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

    glm::ivec2 buffer_size_;

    gl::RenderQuad render_quad_;

    bool flight_mode_ = true;
    float vy_ = 0.f;

    std::optional<glm::ivec3> pointing_block_;

    GLuint  render_gbuffer_query_ = 0,
            render_ssao_query_ = 0,
            render_ssaoblur_query_ = 0,
            render_lighting_query_ = 0,
            render_skybox_query_ = 0,
            render_text_query_ = 0;

    JSObjectRef js_Events;
  };
}

#endif
