#ifndef __CHUNKLANDS_GAME_H__
#define __CHUNKLANDS_GAME_H__

#include <chunklands/debug.hxx>
#include <chunklands/js.hxx>
#include <chunklands/engine/IScene.hxx>
#include <chunklands/engine/ICollisionSystem.hxx>
#include <chunklands/engine/Camera.hxx>
#include <chunklands/engine/Window.hxx>
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/engine/SSAOPass.hxx>
#include <chunklands/engine/SSAOBlurPass.hxx>
#include <chunklands/engine/LightingPass.hxx>
#include <chunklands/engine/SkyboxPass.hxx>
#include <chunklands/engine/TextRenderer.hxx>
#include <chunklands/engine/GameOverlayRenderer.hxx>
#include <chunklands/engine/BlockSelectPass.hxx>
#include <chunklands/engine/Skybox.hxx>
#include <chunklands/engine/Camera.hxx>
#include <chunklands/engine/IScene.hxx>
#include <chunklands/engine/MovementController.hxx>
#include <chunklands/gl/RenderQuad.hxx>
#include <chunklands/math.hxx>
#include <glm/vec3.hpp>
#include <queue>
#include <unordered_map>

namespace chunklands::modules::game {







  // struct block_collision {
  //   engine::collision_result collision_result;
  //   const BlockDefinition* block_def;
  //   math::ivec3 pos_in_chunk;
  //   Chunk *chunk;
  // };


}

#endif
