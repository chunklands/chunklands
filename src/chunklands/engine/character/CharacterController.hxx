#ifndef __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__

#include <chunklands/engine/EngineGameData.hxx>
#include <chunklands/engine/camera/Camera.hxx>
#include <chunklands/engine/collision/MovementController.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::character {

class CharacterController {
public:
    CharacterController(
        camera::Camera& camera, EngineChunkData& engine_chunk_data, EngineGameData& engine_game_data)
        : camera_(camera)
        , engine_chunk_data_(engine_chunk_data)
        , engine_game_data_(engine_game_data)
    {
    }

    void MoveAndLook(double diff, double now, const glm::vec2& forward_right, const glm::vec2& look_delta, bool jump);

    void SetCollision(bool collision)
    {
        collision_ = collision;
    }

    bool IsCollision() const
    {
        return collision_;
    }

    void SetFlightMode(bool flight_mode)
    {
        flight_mode_ = flight_mode;

        if (!flight_mode) {
            last_ground_time_ = glfwGetTime();
        }
    }

    bool IsFlightMode() const
    {
        return flight_mode_;
    }

private:
    camera::Camera& camera_;
    EngineChunkData& engine_chunk_data_;
    EngineGameData& engine_game_data_;

    collision::MovementController movement_controller_;
    bool collision_ = true;
    bool flight_mode_ = true;
    bool is_grounded_ = true;
    double last_ground_time_ = 0.0;
};

} // namespace chunklands::engine::character

#endif