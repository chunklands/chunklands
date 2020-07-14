#ifndef __CHUNKLANDS_GAME_WORLD_HXX__
#define __CHUNKLANDS_GAME_WORLD_HXX__

#include <chunklands/js.hxx>
#include <chunklands/engine/ICollisionSystem.hxx>
#include <chunklands/engine/Camera.hxx>
#include "ChunkGenerator.hxx"
#include "BlockRegistrar.hxx"

namespace chunklands::game {

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
    collision::collision_impulse ProcessNextCollision(const math::fAABB3& box, const math::fvec3& movement) override;
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

} // namespace chunklands::game

#endif