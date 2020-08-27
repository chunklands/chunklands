#ifndef __CHUNKLANDS_API_SHARED_HXX__
#define __CHUNKLANDS_API_SHARED_HXX__

#include "Engine.hxx"
#include "EngineChunkData.hxx"
#include <chunklands/engine/camera/Camera.hxx>
#include <chunklands/engine/character/CharacterController.hxx>
#include <chunklands/engine/chunk/Chunk.hxx>
#include <chunklands/engine/engine_exception.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/engine/render/LightingPass.hxx>
#include <chunklands/engine/window/Window.hxx>
#include <chunklands/engine/window/WindowInputController.hxx>
#include <map>
#include <set>
#include <thread>

namespace chunklands::engine {

struct EngineData {

    struct {
        std::thread opengl_thread;
        boost::loop_executor opengl;

        // boost::
        bool stop = false;
    } executors;

    struct {
        std::set<window::Window*> windows;
        std::map<window::Window*, window::WindowInputController*> window_input_controllers;
        window::WindowInputController* current_window_input_controller = nullptr;
    } window;

    struct {
        camera::Camera camera;
    } camera;

    EngineChunkData chunk;

    character::CharacterController character_controller { camera.camera, chunk };

    struct {
        int render_refresh_rate = -1;
    } gameloop;

    struct {
        bool initialized = false;
        bool start_poll_events = false;
    } glfw;

    struct {
        gl::RenderQuad* render_quad = nullptr;
        render::GBufferPass* gbuffer = nullptr;
        render::LightingPass* lighting = nullptr;
    } render_pipeline;

    struct {
        std::set<void*> unbaked;
        std::set<block::Block*> blocks;
    } block;
};

} // namespace chunklands::engine

#endif