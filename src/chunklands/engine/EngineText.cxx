
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/text/generate_text_mesh.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CETextHandle*> Engine::TextCreate(CEFontHandle* font)
{
    EASY_FUNCTION();
    ENGINE_FN();

    auto text = std::make_unique<text::Text>(font);
    return EnqueueTask(data_->executors.opengl, [this, text = std::move(text)]() mutable -> EngineResultX<CETextHandle*> {
        ENGINE_CHECK(has_handle(data_->font.fonts, text->font));
        ENGINE_CHECK_MSG(data_->text.texts.insert(text.get()).second, "insert text");

        return Ok(reinterpret_cast<CETextHandle*>(text.release()));
    });
}

AsyncEngineResult<CENone> Engine::TextUpdate(CETextHandle* handle, CETextUpdate update)
{
    EASY_FUNCTION();
    ENGINE_FN();
    return EnqueueTask(data_->executors.opengl, [this, handle, update = std::move(update)]() -> EngineResultX<CENone> {
        text::Text* text = nullptr;
        ENGINE_CHECK(get_handle(&text, data_->text.texts, handle));

        bool update_vao = false;

        if (update.text) {
            text->text = *update.text;
            update_vao = true;
        }

        if (update.pos) {
            text->pos.x = update.pos->x;
            text->pos.y = update.pos->y;
        }

        if (update_vao) {
            font::Font* font = nullptr;
            ENGINE_CHECK(get_handle(&font, data_->font.fonts, text->font));
            text::generate_text_mesh(font, text);
        }

        return Ok();
    });
}

} // namespace chunklands::engine