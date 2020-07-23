
#include "SpriteRegistrar.hxx"

namespace chunklands::game {

  JS_DEF_WRAP(SpriteRegistrar)

  void SpriteRegistrar::JSCall_addSprite(JSCbi info) {
    auto&& sprite = std::make_unique<sprite_definition>();

    auto&& js_model = info[0].ToObject();
    sprite->id = js_model.Get("id").ToString();

    auto&& js_faces = js_model.Get("faces").ToObject();
    auto&& js_keys = js_faces.GetPropertyNames();

    for (uint32_t i = 0; i < js_keys.Length(); i++) {
      auto&& js_key = js_keys.Get(i);
      auto&& js_face = js_faces.Get(js_key).As<JSArray>();
      std::vector<GLfloat> face;
      face.reserve(js_face.Length());

      for (uint32_t i = 0; i < js_face.Length(); i++) {
        face.push_back(js_face.Get(i).ToNumber());
      }

      sprite->faces.insert(std::make_pair(js_key.ToString(), std::move(face)));
    }

    sprites_.insert(std::make_pair(sprite->id, std::move(sprite)));
  }

  const sprite_definition* SpriteRegistrar::GetSprite(const std::string& id) const {
    auto&& it = sprites_.find(id);
    if (it == sprites_.end()) {
      return nullptr;
    }

    return it->second.get();
  }

} // namespace chunklands::game