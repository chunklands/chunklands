
#include "Engine.hxx"
#include "EngineData.hxx"

namespace chunklands::engine {

EngineResultX<CENone> Engine::CharacterSetCollision(bool collision)
{
    data_->character_controller.SetCollision(collision);
    return Ok();
}

EngineResultX<bool> Engine::CharacterIsCollision()
{
    return Ok(data_->character_controller.IsCollision());
}

} // namespace chunklands::engine