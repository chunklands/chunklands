#ifndef __CHUNKLANDS_ENGINE_RENDER_BLOCKSELECTPASS_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_BLOCKSELECTPASS_HXX__

#include <boost/move/core.hpp>
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/Uniform.hxx>
#include <chunklands/engine/gl/Vao.hxx>
#include <chunklands/engine/window/Window.hxx>
#include <memory>

namespace chunklands::engine::render {

class BlockSelectPass {

public:
    BlockSelectPass(std::unique_ptr<gl::Program> program);
    ~BlockSelectPass();

    void MakePass(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos);

private:
    std::unique_ptr<gl::Program> program_;

    gl::Vao<GL_TRIANGLES, CEVaoElementBlockSelectBlock> vao;

    gl::Uniform<glm::mat4> u_proj_;
    gl::Uniform<glm::mat4> u_view_;

    boost::signals2::scoped_connection window_resize_conn_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(BlockSelectPass)
};

} // namespace chunklands::engine::render

#endif