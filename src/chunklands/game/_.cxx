
#include "_.hxx"

#include <chunklands/jsexport.hxx>

#include "BlockRegistrar.hxx"
#include "SpriteRegistrar.hxx"
#include "ChunkGenerator.hxx"
#include "Scene.hxx"
#include "World.hxx"
#include "GameOverlay.hxx"

namespace chunklands::game {

  JSObject Init(JSEnv env, JSObject exports) {
    JS_EXPORT(BlockRegistrar);
    JS_EXPORT(SpriteRegistrar);
    JS_EXPORT(ChunkGenerator);
    JS_EXPORT(Scene);
    JS_EXPORT(World);
    JS_EXPORT(GameOverlay);
    return exports;
  }

} // namespace chunklands::game