#ifndef __CHUNKLANDS_API_SHARED_HXX__
#define __CHUNKLANDS_API_SHARED_HXX__

#include <chunklands/engine/Camera.hxx>
#include <chunklands/engine/CharacterController.hxx>
#include <chunklands/engine/Chunk.hxx>
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/engine/LightingPass.hxx>
#include <chunklands/engine/Window.hxx>
#include <chunklands/engine/WindowInputController.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine_exception.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/libcxx/boost_thread.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <glm/vec3.hpp>
#include <map>
#include <set>
#include <thread>
#include <unordered_map>

#define _ENGINE_FN_NAME __api_fn_name
#define ENGINE_FN() static const char* _ENGINE_FN_NAME = BOOST_CURRENT_FUNCTION;

#define ENGINE_CHECK(x) ENGINE_CHECK_MSG((x), #x)
#define ENGINE_CHECK_MSG(x, MSG)                                       \
    do {                                                               \
        if (!(x)) {                                                    \
            return Err(create_engine_exception(_ENGINE_FN_NAME, MSG)); \
        }                                                              \
    } while (0)

namespace chunklands::engine {

struct ivec3_hasher {
    std::size_t operator()(const glm::ivec3& v) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, boost::hash_value(v.x));
        boost::hash_combine(seed, boost::hash_value(v.y));
        boost::hash_combine(seed, boost::hash_value(v.z));

        return seed;
    }
};

struct EngineData {

    struct {
        std::thread opengl_thread;
        boost::loop_executor opengl;
        bool stop = false;
    } executors;

    struct {
        std::set<Window*> windows;
        std::map<Window*, WindowInputController*> window_input_controllers;
        WindowInputController* current_window_input_controller = nullptr;
    } window;

    struct {
        Camera camera;
    } camera;

    struct {
        std::set<Chunk*> chunks;
        std::set<Chunk*> by_state[ChunkState::kCount];
        std::set<Chunk*> scene;
        std::unordered_map<glm::ivec3, Chunk*, ivec3_hasher> by_pos;
    } chunk;

    CharacterController character_controller { &camera.camera };

    struct {
        int render_refresh_rate = -1;
    } gameloop;

    struct {
        bool initialized = false;
        bool start_poll_events = false;
    } glfw;

    struct {
        gl::RenderQuad* render_quad = nullptr;
        GBufferPass* gbuffer = nullptr;
        LightingPass* lighting = nullptr;
    } render_pipeline;

    struct {
        std::set<void*> unbaked;
        std::set<Block*> blocks;
    } block;
};

template <class C, class T>
inline bool has_handle(const C& container, const T& element)
{
    const typename C::value_type v = reinterpret_cast<typename C::value_type>(element);
    return container.find(v) != container.end();
}

template <class F, class R = std::result_of_t<F && ()>>
inline boost::future<R> EnqueueTask(boost::loop_executor& executor, F&& fn)
{

    boost::packaged_task<R()> task(std::forward<F>(fn));
    boost::future<R> result = task.get_future();
    executor.submit(std::move(task));

    return result;
}

} // namespace chunklands::engine

#endif