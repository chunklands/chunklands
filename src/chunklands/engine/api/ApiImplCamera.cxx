
#include "../Api.hxx"
#include "api-shared.hxx"

namespace chunklands::engine {

  boost::future<void>
  Api::CameraAttachWindow(CEWindowHandle* handle) {
    API_FN();
    Window* window = reinterpret_cast<Window*>(handle);
    CHECK(has_handle(api_data(data_)->windows, window));
    return EnqueueTask(executor_, [this, window]() {

      auto it = api_data(data_)->window_input_controllers.find(window);
      CHECK(it != api_data(data_)->window_input_controllers.end());

      window->StartMouseGrab();
      api_data(data_)->current_window_input_controller = it->second;
    });
  }

  boost::future<void>
  Api::CameraDetachWindow(CEWindowHandle* handle) {
    API_FN();
    Window* window = reinterpret_cast<Window*>(handle);
    CHECK(has_handle(api_data(data_)->windows, window));
    return EnqueueTask(executor_, [this, window]() {

      auto it = api_data(data_)->window_input_controllers.find(window);
      CHECK(it != api_data(data_)->window_input_controllers.end());

      window->StopMouseGrab();
      if (api_data(data_)->current_window_input_controller == it->second) {
        api_data(data_)->current_window_input_controller = nullptr;
      }
    });
  }

  boost::future<CECameraPosition>
  Api::CameraGetPosition() {
    return EnqueueTask(executor_, [this]() {
      const glm::vec3& eye = api_data(data_)->camera.camera.GetEye();

      return CECameraPosition {
        .x = eye.x,
        .y = eye.y,
        .z = eye.z
      };
    });
  }

  boost::signals2::scoped_connection
  Api::CameraOn(const std::string& event, std::function<void(CECameraEvent)> callback) {
    if (event == "positionchange") {
      return api_data(data_)->camera.camera.on_position_change.connect([callback = std::move(callback)](CECameraPosition pos) {
        CECameraEvent event("positionchange");
        event.positionchange = std::move(pos);
        callback(std::move(event));
      });
    }

    return boost::signals2::scoped_connection();
  }

} // namespace chunklands::engine