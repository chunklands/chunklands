#ifndef __CHUNKLANDS_ENGINE_ALGORITHM_TEXTUREBAKE_HXX__
#define __CHUNKLANDS_ENGINE_ALGORITHM_TEXTUREBAKE_HXX__

#include <chunklands/engine/block/Block.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/stb.hxx>
#include <functional>
#include <map>

namespace chunklands::engine::algorithm::texture_bake {

class TextureBakerEntity {
public:
    virtual ~TextureBakerEntity() {}

    virtual const stbi_uc* GetTextureData() const = 0;
    virtual int GetTextureWidth() const = 0;
    virtual int GetTextureHeight() const = 0;
    virtual int GetTextureChannels() const = 0;
    virtual void ForEachUVs(const std::function<void(float&, float&)>&) = 0;
};

struct bake_result {
    std::vector<unsigned char> texture;
    int width;
    int height;
};

class TextureBaker {
public:
    bool AddEntity(void* ptr, std::unique_ptr<TextureBakerEntity> texture_entity)
    {
        auto result = entities_.insert(std::make_pair(ptr, std::move(texture_entity)));
        return result.second;
    }

    bool RemoveEntity(void* ptr)
    {
        const size_t erase_count = entities_.erase(ptr);
        return erase_count == 1;
    }

    bake_result Bake();

private:
    std::map<void*, std::unique_ptr<TextureBakerEntity>> entities_;
};

} // namespace chunklands::engine::algorithm::texture_bake

#endif