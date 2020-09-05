#ifndef __CHUNKLANDS_ENGINE_ENGINEGAMEDATA_HXX__
#define __CHUNKLANDS_ENGINE_ENGINEGAMEDATA_HXX__

#include <boost/signals2.hpp>
#include <glm/vec3.hpp>
#include <optional>

namespace chunklands::engine {

class EngineGameData {
public:
    void UpdatePointingBlock(std::optional<glm::ivec3> p);
    const std::optional<glm::ivec3>& GetPointingBlock() const
    {
        return pointing_block;
    }

public:
    boost::signals2::signal<void(const std::optional<glm::ivec3>&)> on_pointingblock_change;

private:
    std::optional<glm::ivec3> pointing_block;
};

} // namespace chunklands::engine

#endif