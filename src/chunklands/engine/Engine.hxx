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
    AsyncEngineResult<CENone> GLFWInit();
    EngineResultX<CENone> GLFWPollEvents();

    AsyncEngineResult<CEWindowHandle*> WindowCreate(int width, int height, std::string title);
    AsyncEngineResult<CENone> WindowLoadGL(CEWindowHandle* handle);
    EngineResultX<EventConnection> WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> main_thread_callback);
    EngineResultX<CESize2i> WindowGetSize(CEWindowHandle* handle);
    EngineResultX<CEWindowContentSize> WindowGetContentSize(CEWindowHandle* handle);

    AsyncEngineResult<CENone> RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init);

    AsyncEngineResult<CEBlockHandle*> BlockCreate(CEBlockCreateInit init);
    AsyncEngineResult<CEBlockBakeResult> BlockBake();

    AsyncEngineResult<CESpriteHandle*> SpriteCreate(CESpriteCreateInit init);

    AsyncEngineResult<CEChunkHandle*> ChunkCreate(int x, int y, int z);
    AsyncEngineResult<CENone> ChunkDelete(CEChunkHandle* handle);
    AsyncEngineResult<CENone> ChunkUpdateData(CEChunkHandle* handle, CEBlockHandle** blocks);

    AsyncEngineResult<CENone> SceneAddChunk(CEChunkHandle* handle);
    AsyncEngineResult<CENone> SceneRemoveChunk(CEChunkHandle* handle);

    AsyncEngineResult<CENone> CameraAttachWindow(CEWindowHandle* handle);
    AsyncEngineResult<CENone> CameraDetachWindow(CEWindowHandle* handle);
    AsyncEngineResult<CECameraPosition> CameraGetPosition();
    EngineResultX<EventConnection> CameraOn(const std::string& event, std::function<void(CECameraEvent)> opengl_thread_callback);

    EngineResultX<CENone> CharacterSetCollision(bool collision);
    EngineResultX<bool> CharacterIsCollision();
    EngineResultX<CENone> CharacterSetFlightMode(bool flight_mode);
    EngineResultX<bool> CharacterIsFlightMode();

    AsyncEngineResult<CEFontHandle*> FontLoad(CEFontInit init);

    AsyncEngineResult<CETextHandle*> TextCreate(CEFontHandle* font);
    AsyncEngineResult<CENone> TextUpdate(CETextHandle* handle, CETextUpdate update);

    EngineResultX<EventConnection> GameOn(const std::string& event, std::function<void(CEGameEvent)> opengl_thread_callback);

    AsyncEngineResult<CESpriteInstanceHandle*> SpriteInstanceCreate(CESpriteHandle* handle);
    AsyncEngineResult<CENone> SpriteInstanceUpdate(CESpriteInstanceHandle* handle, CESpriteInstanceUpdate update);

private:
    EngineData* data_ = nullptr;
};

} // namespace chunklands::engine

#endif