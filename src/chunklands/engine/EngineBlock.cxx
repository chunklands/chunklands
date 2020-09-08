
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/block/Block.hxx>
#include <chunklands/engine/image/image_loader.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/stb.hxx>

namespace chunklands::engine {

struct stb_image_delete {
    void operator()(stbi_uc* data) const
    {
        if (data != nullptr) {
            stbi_image_free(data);
        }
    }
};

using unique_ptr_stb_image = std::unique_ptr<stbi_uc, stb_image_delete>;

struct unbaked_block {
    block::Block* block = nullptr;
    unique_ptr_stb_image image;
    int width = 0;
    int height = 0;
};

constexpr int COLOR_COMPONENTS = 4;

class BlockTextureBakerEntity : public algorithm::texture_bake::TextureBakerEntity {
public:
    BlockTextureBakerEntity(const std::set<block::Block*>* blocks, block::Block* block, image::image image)
        : blocks_(blocks)
        , block_(block)
        , image_(std::move(image))
    {
        assert(blocks != nullptr);
        assert(block != nullptr);
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
        assert(has_handle(*blocks_, block_));

        for (auto& face : block_->faces) {
            for (auto& data_elem : face.data) {
                uv_transform(data_elem.uv[0], data_elem.uv[1]);
            }
        }
    }

private:
    const std::set<block::Block*>* blocks_ = nullptr;
    block::Block* block_ = nullptr;
    image::image image_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(BlockTextureBakerEntity)
};

AsyncEngineResult<CEBlockHandle*>
Engine::BlockCreate(CEBlockCreateInit init)
{
    EASY_FUNCTION();
    ENGINE_FN();
    ENGINE_CHECK(init.id.length() > 0);

    auto block = std::make_unique<block::Block>(std::move(init.id), init.opaque, std::move(init.faces));
    auto image = image::load_image(init.texture.data(), init.texture.size(), COLOR_COMPONENTS);
    auto entity = std::unique_ptr<algorithm::texture_bake::TextureBakerEntity>(
        new BlockTextureBakerEntity(&data_->block.blocks, block.get(), std::move(image)));

    return EnqueueTask(data_->executors.opengl, [this, block = std::move(block), entity = std::move(entity)]() mutable -> EngineResultX<CEBlockHandle*> {
        auto insert_result = data_->block.blocks.insert(block.get());
        ENGINE_CHECK_MSG(insert_result.second, "insert block");

        ENGINE_CHECK(data_->texture_baker.AddEntity(block.get(), std::move(entity)));
        return Ok(reinterpret_cast<CEBlockHandle*>(block.release()));
    });
}

AsyncEngineResult<CENone>
Engine::BlockBake()
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this]() -> EngineResultX<CENone> {
        ENGINE_CHECK(data_->render.gbuffer != nullptr);

        auto result = data_->texture_baker.Bake();
        data_->render.gbuffer->LoadTexture(result.width, result.height, GL_RGBA, GL_UNSIGNED_BYTE, result.texture.data());

        for (sprite::Sprite* const sprite : data_->sprite.sprites) {
            sprite->vao.Initialize(
                sprite->vao_elements.data(), sprite->vao_elements.size());
        }

        return Ok();
    });
}

} // namespace chunklands::engine