
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

} // namespace chunklands::engine