
#include "EngineGameData.hxx"

namespace chunklands::engine {

void EngineGameData::UpdatePointingBlock(std::optional<glm::ivec3> p)
{
    const bool changed = pointing_block != p;
    pointing_block = p;

    if (changed) {
        on_pointingblock_change(p);
    }
}

} // namespace chunklands::engine