
#include "Engine.hxx"
#include "EngineData.hxx"
#include <chunklands/libcxx/ThreadGuard.hxx>

namespace chunklands::engine {

EngineResultX<EventConnection> Engine::GameOn(const std::string& event, std::function<void(CEGameEvent)> callback)
{
    if (event == "pointingblockchange") {
        return Ok(EventConnection(data_->game.on_pointingblock_change.connect([callback = std::move(callback)](const std::optional<glm::ivec3>& pointing_block) {
            assert(libcxx::ThreadGuard::IsOpenGLThread());
            CEGameEvent event("pointingblockchange");
            if (pointing_block) {
                const glm::ivec3& v = *pointing_block;
                event.pointingblockchange = CEVector3i { .x = v.x, .y = v.y, .z = v.z };
            }
            callback(std::move(event));
        })));
    }

    return Ok();
}

} // namespace chunklands::engine