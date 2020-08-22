#ifndef __CHUNKLANDS_ENGINE_ENGINE_ENGINE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_ENGINE_HXX__

#include <boost/signals2.hpp>
#include <chunklands/engine/engine_types.hxx>
#include <chunklands/libcxx/boost_thread.hxx>

namespace chunklands::engine {

  struct EngineData;

  class Engine {
  public:
    Engine();
    ~Engine();

  public:
    void Render();
    void RenderSwap();
    void Update();
    void Terminate();

  public:
    boost::future<void>                 GLFWInit();
    void                                GLFWStartPollEvents(bool poll);
    bool                                GLFWStartPollEvents() const;
    
    boost::future<CEWindowHandle*>      WindowCreate(int width, int height, std::string title);
    boost::future<void>                 WindowLoadGL(CEWindowHandle* handle);
    boost::signals2::scoped_connection  WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> callback);
    
    boost::future<void>                 RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init);
    
    boost::future<CEBlockHandle*>       BlockCreate(CEBlockCreateInit init);
    boost::future<void>                 BlockBake();

    boost::future<CEChunkHandle*>       ChunkCreate(int x, int y, int z);
    boost::future<void>                 ChunkDelete(CEChunkHandle* handle);
    boost::future<void>                 ChunkUpdateData(CEChunkHandle* handle, CEBlockHandle** blocks);

    boost::future<void>                 SceneAddChunk(CEChunkHandle* handle);
    boost::future<void>                 SceneRemoveChunk(CEChunkHandle* handle);

    boost::future<void>                 CameraAttachWindow(CEWindowHandle* handle);
    boost::future<void>                 CameraDetachWindow(CEWindowHandle* handle);
    boost::future<CECameraPosition>     CameraGetPosition();
    boost::signals2::scoped_connection  CameraOn(const std::string& event, std::function<void(CECameraEvent)> callback);

  private:
    EngineData* data_ = nullptr;
  };

} // namespace chunklands::engine

#endif