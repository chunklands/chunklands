
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/algorithm/texture_bake.hxx>
#include <chunklands/engine/image/image_loader.hxx>
#include <chunklands/engine/sprite/Sprite.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/stb.hxx>

namespace chunklands::engine {

constexpr int COLOR_COMPONENTS = 4;

class SpriteTextureBakerEntity : public algorithm::texture_bake::TextureBakerEntity {
public:
    SpriteTextureBakerEntity(const std::set<sprite::Sprite*>* sprites, sprite::Sprite* sprite, image::image image)
        : sprites_(sprites)
        , sprite_(sprite)
        , image_(std::move(image))
    {
        assert(sprites != nullptr);
        assert(sprite != nullptr);
    }

    const stbi_uc* GetTextureData() const override
    {
        return image_.data.get();
    }

    int GetTextureWidth() const override
    {
        return image_.width;
    }

    int GetTextureHeight() const override
    {
        return image_.height;
    }

    int GetTextureChannels() const override
    {
        return image_.channels;
    }

    void ForEachUVs(const std::function<void(float&, float&)>& uv_transform) override
    {
        assert(has_handle(*sprites_, sprite_));

        for (auto& data_elem : sprite_->vao_elements) {
            uv_transform(data_elem.uv[0], data_elem.uv[1]);
        }
    }

private:
    const std::set<sprite::Sprite*>* sprites_ = nullptr;
    sprite::Sprite* sprite_ = nullptr;
    image::image image_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(SpriteTextureBakerEntity)
};

AsyncEngineResult<CESpriteHandle*>
Engine::SpriteCreate(CESpriteCreateInit init)
{
    EASY_FUNCTION();
    ENGINE_FN();
    ENGINE_CHECK(init.id.length() > 0);

    auto sprite = std::make_unique<sprite::Sprite>(std::move(init.id), std::move(init.data));
    auto image = image::load_image(init.texture.data(), init.texture.size(), COLOR_COMPONENTS);
    auto entity = std::unique_ptr<algorithm::texture_bake::TextureBakerEntity>(
        new SpriteTextureBakerEntity(&data_->sprite.sprites, sprite.get(), std::move(image)));

    return EnqueueTask(data_->executors.opengl, [this, sprite = std::move(sprite), entity = std::move(entity)]() mutable -> EngineResultX<CESpriteHandle*> {
        auto insert_result = data_->sprite.sprites.insert(sprite.get());
        ENGINE_CHECK_MSG(insert_result.second, "insert sprite");

        ENGINE_CHECK(data_->texture_baker.AddEntity(sprite.get(), std::move(entity)));
        return Ok(reinterpret_cast<CESpriteHandle*>(sprite.release()));
    });
}
} // namespace chunklands::engine