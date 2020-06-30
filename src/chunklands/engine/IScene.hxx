#ifndef __CHUNKLANDS_ENGINE_ISCENE_HXX__
#define __CHUNKLANDS_ENGINE_ISCENE_HXX__

namespace chunklands::engine {

  class IScene {
  public:
    virtual ~IScene() {}

  public:
    virtual void Prepare() = 0;
    virtual void Update(double update_diff) = 0;
    virtual void Render(double render_diff) = 0;
  };

} // namespace chunklands::engine

#endif