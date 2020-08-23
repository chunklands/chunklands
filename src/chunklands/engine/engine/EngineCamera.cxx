
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::CameraAttachWindow(CEWindowHandle* handle)
{
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        ENGINE_CHECKX(has_handle(data_->window.windows, handle));
        Window* window = reinterpret_cast<Window*>(handle);

        auto it = data_->window.window_input_controllers.find(window);
        ENGINE_CHECKX(it != data_->window.window_input_controllers.end());

        window->StartMouseGrab();
        data_->window.current_window_input_controller = it->second;

        return Ok();
    });
}

AsyncEngineResult<CENone>
Engine::CameraDetachWindow(CEWindowHandle* handle)
{
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        ENGINE_CHECKX(has_handle(data_->window.windows, handle));
        Window* window = reinterpret_cast<Window*>(handle);

        auto it = data_->window.window_input_controllers.find(window);
        ENGINE_CHECKX(it != data_->window.window_input_controllers.end());

        window->StopMouseGrab();
        if (data_->window.current_window_input_controller == it->second) {
            data_->window.current_window_input_controller = nullptr;
        }

        return Ok();
    });
}

AsyncEngineResult<CECameraPosition>
Engine::CameraGetPosition()
{
    return EnqueueTask(data_->executors.opengl, [this]() -> EngineResultX<CECameraPosition> {
        const glm::vec3& eye = data_->camera.camera.GetEye();

        return Ok(CECameraPosition {
            .x = eye.x,
            .y = eye.y,
            .z = eye.z });
    });
}

EventConnection
Engine::CameraOn(const std::string& event, std::function<void(CECameraEvent)> callback)
{
    if (event == "positionchange") {
        return data_->camera.camera.on_position_change.connect([callback = std::move(callback)](CECameraPosition pos) {
            CECameraEvent event("positionchange");
            event.positionchange = std::move(pos);
            callback(std::move(event));
        });
    }

    return EventConnection();
}

} // namespace chunklands::engine