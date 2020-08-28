#ifndef __CHUNKLANDS_ENGINE_ENGINERENDERDATA_HXX__
#define __CHUNKLANDS_ENGINE_ENGINERENDERDATA_HXX__

#include <boost/signals2.hpp>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/render/BlockSelectPass.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/engine/render/LightingPass.hxx>

namespace chunklands::engine {

struct EngineRenderData {
    gl::RenderQuad* render_quad = nullptr;
    render::GBufferPass* gbuffer = nullptr;
    render::LightingPass* lighting = nullptr;
    render::BlockSelectPass* block_select = nullptr;

    std::optional<glm::ivec3> pointing_block;

    glm::mat4 proj;
    boost::signals2::scoped_connection window_resize_conn;
};

} // namespace chunklands::engine

#endif