
#include "generate_text_mesh.hxx"
#include <boost/regex/pending/unicode_iterator.hpp>
#include <glm/vec2.hpp>

constexpr int REFERENCE_HEIGHT = 64;

namespace chunklands::engine::text {

void generate_text_mesh(const font::Font* font, text::Text* text)
{
    assert(text != nullptr);
    assert(font != nullptr);
    assert(text->font == reinterpret_cast<const CEFontHandle*>(font));

    const size_t elem_count = text->text.length() * 6;
    std::vector<CEVaoElementText> vb_data(elem_count);
    assert(vb_data.size() == elem_count);

    const float scale = float(REFERENCE_HEIGHT) / float(font->size);

    glm::ivec2 pos(0, 0);

    unsigned i = 0;
    for (boost::u8_to_u32_iterator it(text->text.begin()), end(text->text.end());
         it != end; it++) {
        unsigned c = *it;
        if (c == '\n') {
            pos.x = 0;
            pos.y -= REFERENCE_HEIGHT;
            continue;
        }

        auto it_char = font->characters.find(c);
        if (it_char == font->characters.end()) {
            continue;
        }

        const CEFontInitCharacter& ch = it_char->second;
        const glm::vec2 v0 = glm::vec2(pos.x - ch.originX, pos.y - ch.height + ch.originY) * scale;
        const glm::vec2 v1 = v0 + glm::vec2(ch.width, ch.height) * scale;

        const glm::vec2 uv0(float(ch.x) / font->width,
            1.f - float(-ch.y - ch.height) / font->height);
        const glm::vec2 uv1 = uv0
            + glm::vec2(float(ch.width) / font->width,
                float(-ch.height) / font->height);

        assert(i % 6 == 0);

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v0.x;
            elem.position[1] = v0.y;
            elem.uv[0] = uv0.x;
            elem.uv[1] = uv0.y;
            i++;
        }

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v0.x;
            elem.position[1] = v1.y;
            elem.uv[0] = uv0.x;
            elem.uv[1] = uv1.y;
            i++;
        }

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v1.x;
            elem.position[1] = v1.y;
            elem.uv[0] = uv1.x;
            elem.uv[1] = uv1.y;
            i++;
        }

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v0.x;
            elem.position[1] = v0.y;
            elem.uv[0] = uv0.x;
            elem.uv[1] = uv0.y;
            i++;
        }

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v1.x;
            elem.position[1] = v1.y;
            elem.uv[0] = uv1.x;
            elem.uv[1] = uv1.y;
            i++;
        }

        {
            assert(i < vb_data.size());
            auto& elem = vb_data[i];
            elem.position[0] = v1.x;
            elem.position[1] = v0.y;
            elem.uv[0] = uv1.x;
            elem.uv[1] = uv0.y;
            i++;
        }

        pos.x += ch.advance;
    }

    text->vao.Initialize(vb_data.data(), vb_data.size());
}

} // namespace chunklands::engine::text