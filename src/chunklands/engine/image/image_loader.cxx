
#include "image_loader.hxx"

namespace chunklands::engine::image {

image load_image(unsigned char* content, size_t size, int desired_channels)
{
    image img;
    img.data.reset(stbi_load_from_memory(content, size,
        &img.width, &img.height, &img.channels, desired_channels));
    return img;
}

} // namespace chunklands::engine::image