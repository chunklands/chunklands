
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/sprite/Sprite.hxx>
#include <chunklands/engine/spriteinstance/SpriteInstance.hxx>

namespace chunklands::engine {

AsyncEngineResult<CESpriteInstanceHandle*> Engine::SpriteInstanceCreate(CESpriteHandle* handle)
{
    ENGINE_FN();
    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CESpriteInstanceHandle*> {
        sprite::Sprite* sprite = nullptr;
        ENGINE_CHECK(get_handle(&sprite, data_->sprite.sprites, handle));

        auto sprite_instance = std::make_unique<spriteinstance::SpriteInstance>(sprite);
        ENGINE_CHECK(data_->spriteinstance.instances.insert(sprite_instance.get()).second);

        return Ok(reinterpret_cast<CESpriteInstanceHandle*>(sprite_instance.release()));
    });
}

AsyncEngineResult<CENone> Engine::SpriteInstanceUpdate(CESpriteInstanceHandle* handle, CESpriteInstanceUpdate update)
{
    ENGINE_FN();
    return EnqueueTask(data_->executors.opengl, [this, handle, update = std::move(update)]() -> EngineResultX<CENone> {
        spriteinstance::SpriteInstance* instance;
        ENGINE_CHECK(get_handle(&instance, data_->spriteinstance.instances, handle));

        conditional_update(instance->pos.x, update.x);
        conditional_update(instance->pos.y, update.y);
        conditional_update(instance->scale, update.scale);
        conditional_update(instance->show, update.show);

        return Ok();
    });
}

} // namespace chunklands::engine