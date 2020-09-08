#ifndef __CHUNKLANDS_ENGINE_SPRITEINSTANCE_SPRITEINSTANCE_HXX__
#define __CHUNKLANDS_ENGINE_SPRITEINSTANCE_SPRITEINSTANCE_HXX__

#include <chunklands/engine/sprite/Sprite.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::spriteinstance {

struct SpriteInstance {
    SpriteInstance(sprite::Sprite* sprite)
        : sprite(sprite)
    {
    }

    sprite::Sprite* sprite = nullptr;
    glm::vec2 pos {};
    float scale = 0.f;
    bool show = false;
};

} // namespace chunklands::engine::spriteinstance

#endif