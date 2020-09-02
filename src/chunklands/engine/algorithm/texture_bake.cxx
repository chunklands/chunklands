
#include "texture_bake.hxx"
#include <optional>

namespace chunklands::engine::algorithm::texture_bake {

constexpr int COLOR_COMPONENTS = 4;

enum class Split {
    kStillUnknown,
    kHorizontal,
    kVertical
};

struct bake_node {
    std::optional<TextureBakerEntity*> entity;
    std::unique_ptr<bake_node> down, right;
    Split split = Split::kStillUnknown;
    int x = 0, y = 0;
};

struct bake_tree_result {
    std::unique_ptr<bake_node> root;
    int dim;
};

template <class NT>
inline NT nextPOT(NT n)
{
    NT r = 1;
    while (r < n) {
        r *= 2;
    }

    return r;
}

bake_tree_result bake(std::map<void*, std::unique_ptr<TextureBakerEntity>>& u)
{
    std::unique_ptr<bake_node> root = std::make_unique<bake_node>();
    int max_dim = 0;

    for (auto&& item : u) {
        TextureBakerEntity* entity = item.second.get();
        if (entity->GetTextureData() == nullptr) {
            continue;
        }

        if (entity->GetTextureChannels() != COLOR_COMPONENTS) {
            // TODO(daaitch): nothrow => Ok/Err results
            throw std::runtime_error("expected same amount of channels");
        }

        bake_node* node = root.get();

        for (;;) {
            assert(node);

            if (node->split == Split::kStillUnknown) {
                assert(!node->entity);

                node->entity = entity;

                const int overall_width = node->x + entity->GetTextureWidth();
                const int overall_height = node->y + entity->GetTextureHeight();

                max_dim = std::max(max_dim, overall_width);
                max_dim = std::max(max_dim, overall_height);

                node->split = overall_width > overall_height ? Split::kHorizontal : Split::kVertical;
                break;
            }

            // +++++++++
            // +XXX
            // +XXX.....
            // +
            // +
            if (node->split == Split::kHorizontal) {
                assert(node->entity);
                TextureBakerEntity* node_entity = *node->entity;

                if (node_entity->GetTextureHeight() >= entity->GetTextureHeight()) {
                    if (!node->right) {
                        node->right = std::make_unique<bake_node>();
                        node->right->x = node->x + node_entity->GetTextureWidth();
                        node->right->y = node->y;
                    }

                    node = node->right.get();
                    continue;
                }

                if (!node->down) {
                    node->down = std::make_unique<bake_node>();
                    node->down->x = node->x;
                    node->down->y = node->y + node_entity->GetTextureHeight();
                }

                node = node->down.get();
                continue;
            }

            // ++++++
            // +XXX
            // +XXX
            // +  .
            // +  .
            if (node->split == Split::kVertical) {
                assert(node->entity);
                TextureBakerEntity* node_entity = *node->entity;

                if (node_entity->GetTextureWidth() >= entity->GetTextureWidth()) {
                    if (!node->down) {
                        node->down = std::make_unique<bake_node>();
                        node->down->x = node->x;
                        node->down->y = node->y + node_entity->GetTextureHeight();
                    }

                    node = node->down.get();
                    continue;
                }

                if (!node->right) {
                    node->right = std::make_unique<bake_node>();
                    node->right->x = node->x + node_entity->GetTextureWidth();
                    node->right->y = node->y;
                }

                node = node->right.get();
                continue;
            }

            // unreachable
            assert(false);
        }
    }

    const int dim = nextPOT(max_dim);
    assert((dim & (dim - 1)) == 0); // POT check

    return {
        .root = std::move(root),
        .dim = dim
    };
}

void generate_texture(unsigned char* texture, int dim, bake_node& node)
{
    if (!node.entity) {
        return;
    }

    TextureBakerEntity* node_entity = *node.entity;

    const std::size_t off = ((node.x + (node.y * dim)) * COLOR_COMPONENTS);
    unsigned char* dst = texture + off;
    const stbi_uc* src = node_entity->GetTextureData();

    const std::size_t dst_line_offset = dim * COLOR_COMPONENTS;
    const std::size_t src_line_offset = node_entity->GetTextureWidth() * COLOR_COMPONENTS;

    // copy image
    for (int i = 0; i < node_entity->GetTextureHeight(); i++) {
        std::memcpy(dst, src, src_line_offset);
        dst += dst_line_offset;
        src += src_line_offset;
    }

    // update uv
    const GLfloat w = node_entity->GetTextureWidth();
    const GLfloat h = node_entity->GetTextureHeight();

    node_entity->ForEachUVs([&](float& u, float& v) {
        u = (node.x + (u * w)) / dim;
        v = (node.y + (v * h)) / dim;
    });

    // compute other nodes
    if (node.right) {
        generate_texture(texture, dim, *node.right);
    }

    if (node.down) {
        generate_texture(texture, dim, *node.down);
    }
}

bake_result TextureBaker::Bake()
{
    const bake_tree_result result = bake(entities_);

    const std::size_t size = result.dim * result.dim * COLOR_COMPONENTS;
    std::vector<unsigned char> texture(size);
    assert(texture.size() == size);

    generate_texture(texture.data(), result.dim, *result.root);

    const int write_result = stbi_write_png("out.png", result.dim, result.dim, 4, texture.data(), 0);
    assert(write_result == 1);

    entities_.clear();

    return {
        .texture = std::move(texture),
        .width = result.dim,
        .height = result.dim
    };
}

} // namespace chunklands::engine::algorithm::texture_bake