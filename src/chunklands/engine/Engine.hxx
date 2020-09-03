#ifndef __CHUNKLANDS_ENGINE_ENGINE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_HXX__

#include "types.hxx"
#include <boost/signals2.hpp>
#include <chunklands/libcxx/boost_thread.hxx>
#include <variant>

namespace chunklands::engine {

struct EngineData;

class Engine {
public:
    Engine();
    ~Engine();

public:
    void Render(double diff, double now);
    void RenderSwap(double diff, double now);
    void Update(double diff, double now);
    void FinalizeOpenGLThread();
    void Terminate();

public:
    // clang-format off
    AsyncEngineResult<CENone>           GLFWInit();
    EngineResultX<CENone>               GLFWStartPollEvents(bool poll);
    EngineResultX<bool>                 GLFWStartPollEvents() const;
    
    AsyncEngineResult<CEWindowHandle*>  WindowCreate(int width, int height, std::string title);
    AsyncEngineResult<CENone>           WindowLoadGL(CEWindowHandle* handle);
    EngineResultX<EventConnection>      WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> callback);
    
    AsyncEngineResult<CENone>           RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init);
    
    AsyncEngineResult<CEBlockHandle*>   BlockCreate(CEBlockCreateInit init);
    AsyncEngineResult<CENone>           BlockBake();

    AsyncEngineResult<CESpriteHandle*>  SpriteCreate(CESpriteCreateInit init);

    AsyncEngineResult<CEChunkHandle*>   ChunkCreate(int x, int y, int z);
    AsyncEngineResult<CENone>           ChunkDelete(CEChunkHandle* handle);
    AsyncEngineResult<CENone>           ChunkUpdateData(CEChunkHandle* handle, CEBlockHandle** blocks);

    AsyncEngineResult<CENone>           SceneAddChunk(CEChunkHandle* handle);
    AsyncEngineResult<CENone>           SceneRemoveChunk(CEChunkHandle* handle);

    AsyncEngineResult<CENone>           CameraAttachWindow(CEWindowHandle* handle);
    AsyncEngineResult<CENone>           CameraDetachWindow(CEWindowHandle* handle);
    AsyncEngineResult<CECameraPosition> CameraGetPosition();
    EngineResultX<EventConnection>      CameraOn(const std::string& event, std::function<void(CECameraEvent)> callback);

    EngineResultX<CENone>               CharacterSetCollision(bool collision);
    EngineResultX<bool>                 CharacterIsCollision();
    EngineResultX<CENone>               CharacterSetFlightMode(bool flight_mode);
    EngineResultX<bool>                 CharacterIsFlightMode();
    // clang-format on

private:
    EngineData* data_ = nullptr;
};

} // namespace chunklands::engine

#endif