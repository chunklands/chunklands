
#include "image_loader.hxx"
#include <cassert>

namespace chunklands::engine::image {

image load_image(unsigned char* content, size_t size, int desired_channels)
{
    if (size == 0) {
        return {};
    }

    assert(content != nullptr);
    assert(size > 0);
    assert(desired_channels > 0);

    image img;
    img.data.reset(stbi_load_from_memory(content, size,
        &img.width, &img.height, nullptr, desired_channels));

    assert(img.data);

    img.channels = desired_channels;
    return img;
}

} // namespace chunklands::engine::image