
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

EngineResultX<CENone> Engine::CharacterSetFlightMode(bool flight_mode)
{
    data_->character_controller.SetFlightMode(flight_mode);
    return Ok();
}

EngineResultX<bool> Engine::CharacterIsFlightMode()
{
    return Ok(data_->character_controller.IsFlightMode());
}

} // namespace chunklands::engine