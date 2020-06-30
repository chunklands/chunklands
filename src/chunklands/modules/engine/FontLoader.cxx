
#include "FontLoader.hxx"

namespace chunklands::modules::engine {

  JS_DEF_WRAP(FontLoader)

  void FontLoader::JSCall_load(JSCbi info) {
    JS_ASSERT(info.Env(), info.Length() == 2);

    JSHandleScope scope(info.Env());
    
    // texture
    std::string image_path = info[1].ToString();
    texture_.LoadTexture(image_path.c_str());

    // meta data
    meta_.clear();

    auto&& js_meta = info[0].ToObject();
    auto&& js_chars = js_meta.Get("characters").ToObject();
    auto&& js_char_keys = js_chars.GetPropertyNames();
    for (unsigned i = 0; i < js_char_keys.Length(); i++) {
      auto&& js_char_key = js_char_keys.Get(i);
      std::string k = js_char_key.ToString();
      // e.g. "0":{"x":135,"y":35,"width":20,"height":28,"originX":1,"originY":25,"advance":18},
      auto&& js_char_value = js_chars.Get(js_char_key).ToObject();

      std::string key = js_char_key.ToString();

      meta_.insert({key, {
        .pos {
          js_char_value.Get("x").ToNumber().Int32Value(),
          js_char_value.Get("y").ToNumber().Int32Value()
        },
        .size {
          js_char_value.Get("width").ToNumber().Int32Value(),
          js_char_value.Get("height").ToNumber().Int32Value()
        },
        .origin {
          js_char_value.Get("originX").ToNumber().Int32Value(),
          js_char_value.Get("originY").ToNumber().Int32Value()
        },
        .advance = js_char_value.Get("advance").ToNumber().Int32Value(),
      }});
    }
  }

} // namespace chunklands::modules::engine