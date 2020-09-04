#ifndef __CHUNKLANDS_ENGINE_TEXT_GENERATETEXTMESH_HXX__
#define __CHUNKLANDS_ENGINE_TEXT_GENERATETEXTMESH_HXX__

#include <chunklands/engine/font/Font.hxx>
#include <chunklands/engine/text/Text.hxx>
#include <set>

namespace chunklands::engine::text {

void generate_text_mesh(const font::Font* font, text::Text* text);

} // namespace chunklands::engine::text

#endif