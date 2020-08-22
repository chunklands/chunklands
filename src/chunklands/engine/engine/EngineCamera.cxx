
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

  boost::future<void>
  Engine::CameraAttachWindow(CEWindowHandle* handle) {
    ENGINE_FN();
    CHECK_OR_FATAL(has_handle(data_->window.windows, handle));
    Window* window = reinterpret_cast<Window*>(handle);
    return EnqueueTask(data_->executors.opengl, [this, window]() {

      auto it = data_->window.window_input_controllers.find(window);
      CHECK_OR_FATAL(it != data_->window.window_input_controllers.end());

      window->StartMouseGrab();
      data_->window.current_window_input_controller = it->second;
    });
  }

  boost::future<void>
  Engine::CameraDetachWindow(CEWindowHandle* handle) {
    ENGINE_FN();
    CHECK_OR_FATAL(has_handle(data_->window.windows, handle));
    Window* window = reinterpret_cast<Window*>(handle);
    return EnqueueTask(data_->executors.opengl, [this, window]() {

      auto it = data_->window.window_input_controllers.find(window);
      CHECK_OR_FATAL(it != data_->window.window_input_controllers.end());

      window->StopMouseGrab();
      if (data_->window.current_window_input_controller == it->second) {
        data_->window.current_window_input_controller = nullptr;
      }
    });
  }

  boost::future<CECameraPosition>
  Engine::CameraGetPosition() {
    return EnqueueTask(data_->executors.opengl, [this]() {
      const glm::vec3& eye = data_->camera.camera.GetEye();

      return CECameraPosition {
        .x = eye.x,
        .y = eye.y,
        .z = eye.z
      };
    });
  }

  boost::signals2::scoped_connection
  Engine::CameraOn(const std::string& event, std::function<void(CECameraEvent)> callback) {
    if (event == "positionchange") {
      return data_->camera.camera.on_position_change.connect([callback = std::move(callback)](CECameraPosition pos) {
        CECameraEvent event("positionchange");
        event.positionchange = std::move(pos);
        callback(std::move(event));
      });
    }

    return boost::signals2::scoped_connection();
  }

} // namespace chunklands::engine