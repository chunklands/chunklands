
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"

namespace chunklands::engine {

AsyncEngineResult<CEFontHandle*> Engine::FontLoad(CEFontInit init)
{
    EASY_FUNCTION();
    ENGINE_FN();

    auto font_image = image::load_image(init.texture.data(), init.texture.size(), 4);

    return EnqueueTask(data_->executors.opengl, [this, font_image = std::move(font_image), init = std::move(init)]() mutable -> EngineResultX<CEFontHandle*> {
        auto texture = std::make_unique<gl::Texture>(font_image);

        auto font = std::make_unique<font::Font>(init.name, init.size, init.width, init.height,
            std::move(init.characters), std::move(texture));

        ENGINE_CHECK_MSG(data_->font.fonts.insert(font.get()).second, "insert font");
        return Ok(reinterpret_cast<CEFontHandle*>(font.release()));
    });
}

} // namespace chunklands::engine