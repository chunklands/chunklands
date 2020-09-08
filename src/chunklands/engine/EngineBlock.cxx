
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/block/Block.hxx>
#include <chunklands/engine/image/image_loader.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>
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

AsyncEngineResult<CEBlockBakeResult>
Engine::BlockBake()
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this]() -> EngineResultX<CEBlockBakeResult> {
        ENGINE_CHECK(data_->render.gbuffer != nullptr);

        auto result = data_->texture_baker.Bake();
        data_->render.gbuffer->LoadTexture(result.width, result.height, GL_RGBA, GL_UNSIGNED_BYTE, result.texture.data());

        std::unordered_map<std::string, CEBlockHandle*> blocks;

        for (block::Block* const block : data_->block.blocks) {
            std::vector<CEVaoElementSprite> vb_data;
            for (const CEBlockFace& face : block->faces) {
                if (face.type == FaceType::Back || face.type == FaceType::Unknown) {
                    for (auto&& elem : face.data) {
                        vb_data.push_back(CEVaoElementSprite {
                            .position = { elem.position[0], elem.position[1], elem.position[2] },
                            .normal = { elem.normal[0], elem.normal[1], elem.normal[2] },
                            .uv = { elem.uv[0], elem.uv[1] } });
                    }
                }
            }

            std::string sprite_id = "sprite." + block->id;
            LOG(DEBUG) << "auto add sprite '" << sprite_id << "' from block";

            {
                auto sprite = std::make_unique<sprite::Sprite>(std::move(sprite_id), std::move(vb_data));
                auto insert_it = data_->sprite.sprites.insert(sprite.release());
                assert(insert_it.second);
            }

            {
                auto insert_it = blocks.insert({ block->id, reinterpret_cast<CEBlockHandle*>(block) });
                assert(insert_it.second);
            }
        }

        std::unordered_map<std::string, CESpriteHandle*> sprites;

        for (sprite::Sprite* const sprite : data_->sprite.sprites) {
            sprite->vao.Initialize(
                sprite->vao_elements.data(), sprite->vao_elements.size());

            auto insert_it = sprites.insert({ sprite->id, reinterpret_cast<CESpriteHandle*>(sprite) });
            assert(insert_it.second);
        }

        return Ok(CEBlockBakeResult {
            .blocks = std::move(blocks),
            .sprites = std::move(sprites) });
    });
}

} // namespace chunklands::engine