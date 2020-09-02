#ifndef __CHUNKLANDS_ENGINE_IMAGE_IMAGE_LOADER_HXX__
#define __CHUNKLANDS_ENGINE_IMAGE_IMAGE_LOADER_HXX__

#include <chunklands/libcxx/stb.hxx>
#include <memory>

namespace chunklands::engine::image {

static_assert(sizeof(unsigned char) == sizeof(stbi_uc), "check char size");

struct stb_image_delete {
    void operator()(stbi_uc* data) const
    {
        if (data != nullptr) {
            stbi_image_free(data);
        }
    }
};

struct image {
    std::unique_ptr<stbi_uc, stb_image_delete> data;
    int channels;
    int width, height;
};

using unique_ptr_stb_image = std::unique_ptr<stbi_uc, stb_image_delete>;

image load_image(unsigned char* content, size_t size, int desired_channels);

} // namespace chunklands::engine::image

#endif