
#include "shared.hxx"
#include <chunklands/engine/Block.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/libcxx/stb.hxx>

namespace chunklands::engine {

  struct stb_image_delete {
    void operator()(stbi_uc* data) const {
      if (data != nullptr) {
        stbi_image_free(data);
      }
    }
  };

  using unique_ptr_stb_image = std::unique_ptr<stbi_uc, stb_image_delete>;

  struct unbaked_block {
    Block* block = nullptr;
    unique_ptr_stb_image image;
    int width = 0;
    int height = 0;
  };

  constexpr int COLOR_COMPONENTS = 4;

  boost::future<CEBlockHandle*>
  Engine::BlockCreate(CEBlockCreateInit init) {
    EASY_FUNCTION();
    ENGINE_FN();
    CHECK_OR_FATAL(init.id.length() > 0);
    std::unique_ptr<Block> block = std::make_unique<Block>(std::move(init.id), init.opaque, std::move(init.faces));

    static_assert(sizeof(char) == sizeof(stbi_uc), "check char size");
    
    std::unique_ptr<unbaked_block> b = std::make_unique<unbaked_block>();
    b->block = block.get();
    if (init.texture.size() > 0) {
      stbi_uc* d = stbi_load_from_memory(init.texture.data(), init.texture.size(), &b->width, &b->height, nullptr, COLOR_COMPONENTS);
      b->image.reset(d);
      CHECK_OR_FATAL(b->image);
    }

    return EnqueueTask(data_->executors.opengl, [this, block = std::move(block), b = std::move(b)]() mutable {
      data_->block.unbaked.insert(b.release());

      data_->block.blocks.insert(block.get());
      return reinterpret_cast<CEBlockHandle*>(block.release());
    });
  }

  enum BlockBakeNodeSplit {
    kStillUnknown,
    kHorizontal,
    kVertical
  };

  struct block_bake_node {
    unbaked_block* b = nullptr;
    std::unique_ptr<block_bake_node> down, right;
    BlockBakeNodeSplit split = kStillUnknown;
    int x = 0, y = 0;
  };

  long nextPOT(long n) {
    long r = 1;
    while (r < n) {
      r <<= 1;
    }

    return r;
  }

  void generate(unsigned char* texture, int dim, block_bake_node* node) {
    assert(node);

    if (!node->b) {
      return;
    }
    
    const std::size_t    off = ((node->x + (node->y * dim)) * COLOR_COMPONENTS);
    unsigned char*       dst = texture + off;
    const unsigned char* src = node->b->image.get();

    const std::size_t dst_line_offset = dim            * COLOR_COMPONENTS;
    const std::size_t src_line_offset = node->b->width * COLOR_COMPONENTS;

    // copy image
    for (int i = 0; i < node->b->height; i++) {
      std::memcpy(dst, src, src_line_offset);
      dst += dst_line_offset;
      src += src_line_offset;
    }

    // update uv
    const GLfloat d = dim;
    for (CEBlockFace& face : node->b->block->faces) {
      for (CEVaoElementChunkBlock& elem : face.data) {
        const GLfloat x = node->x;
        const GLfloat y = node->y;
        const GLfloat w = node->b->width;
        const GLfloat h = node->b->height;

        elem.uv[0] = (x + (elem.uv[0] * w)) / d;
        elem.uv[1] = (y + (elem.uv[1] * h)) / d;
      }
    }

    // compute other nodes
    if (node->right) {
      generate(texture, dim, node->right.get());
    }

    if (node->down) {
      generate(texture, dim, node->down.get());
    }
  }

  boost::future<void>
  Engine::BlockBake() {
    EASY_FUNCTION();
    ENGINE_FN();
    
    return EnqueueTask(data_->executors.opengl, [this]() {
      CHECK_OR_FATAL(data_->render_pipeline.gbuffer != nullptr);

      std::unique_ptr<block_bake_node> root = std::make_unique<block_bake_node>();
      int max_dim = 0;

      for (void *unbaked : data_->block.unbaked) {
        assert(unbaked);
        unbaked_block* b = reinterpret_cast<unbaked_block*>(unbaked);
        assert(b);

        if (!b->image) {
          continue;
        }

        block_bake_node* node = root.get();
        
        for (;;) {
          assert(node);

          if (node->split == kStillUnknown) {
            assert(node->b == nullptr);
            node->b = b;

            const int overall_width  = node->x + node->b->width;
            const int overall_height = node->y + node->b->height;

            max_dim = std::max(max_dim, overall_width);
            max_dim = std::max(max_dim, overall_height);

            node->split = overall_width > overall_height ? kHorizontal : kVertical;
            break;
          }

          // +++++++++
          // +XXX
          // +XXX.....
          // +
          // +
          if (node->split == kHorizontal) {
            if (node->b->height >= b->height) {
              if (!node->right) {
                node->right = std::make_unique<block_bake_node>();
                node->right->x = node->x + node->b->width;
                node->right->y = node->y;
              }

              node = node->right.get();
              continue;
            }

            if (!node->down) {
              node->down = std::make_unique<block_bake_node>();
              node->down->x = node->x;
              node->down->y = node->y + node->b->height;
            }

            node = node->down.get();
            continue;
          }

          // ++++++
          // +XXX
          // +XXX
          // +  .
          // +  .
          if(node->split == kVertical) {
            if (node->b->width >= b->width) {
              if (!node->down) {
                node->down = std::make_unique<block_bake_node>();
                node->down->x = node->x;
                node->down->y = node->y + node->b->height;
              }

              node = node->down.get();
              continue;
            }

            if (!node->right) {
              node->right = std::make_unique<block_bake_node>();
              node->right->x = node->x + node->b->width;
              node->right->y = node->y;
            }

            node = node->right.get();
            continue;
          }

          // unreachable
          assert(false);
        }
      }

      const long dim = nextPOT(max_dim);
      assert((dim & (dim - 1)) == 0); // POT check

      const std::size_t size = dim * dim * COLOR_COMPONENTS;
      std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(size);
      std::memset(data.get(), 0, size);
      generate(data.get(), dim, root.get());

      CHECK_OR_FATAL(data_->render_pipeline.gbuffer != nullptr);
      // const int result = stbi_write_png("out.png", dim, dim, 4, data.get(), 0);
      // assert(result == 1);
      data_->render_pipeline.gbuffer->LoadTexture(dim, dim, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
    });
  }

} // namespace chunklands::engine