
#include "BlockRegistrar.hxx"

namespace chunklands::game {

  JS_DEF_WRAP(BlockRegistrar)

  JSValue BlockRegistrar::JSCall_addBlock(JSCbi info) {
    JS_ASSERT(info.Env(), info[0].IsObject());

    auto&& js_block_definition = info[0].ToObject();

    auto&& js_id = js_block_definition.Get("id");
    JS_ASSERT(info.Env(), js_id.IsString());
    std::string id = js_id.ToString();

    auto&& js_opaque = js_block_definition.Get("opaque");
    JS_ASSERT(info.Env(), js_opaque.IsBoolean());
    bool opaque = js_opaque.ToBoolean();

    auto&& js_faces = js_block_definition.Get("faces");
    JS_ASSERT(info.Env(), js_faces.IsObject());

    auto&& js_faces_obj = js_faces.As<JSObject>();
    auto&& js_facenames_arr = js_faces_obj.GetPropertyNames();
    std::unordered_map<std::string, std::vector<GLfloat>> faces;

    for (unsigned i = 0; i < js_facenames_arr.Length(); i++) {
      auto&& js_facename = js_facenames_arr.Get(i);
      assert(js_facename.IsString());
      
      auto&& js_facename_str = js_facename.ToString();
      auto&& js_face = js_faces_obj.Get(js_facename_str);
      JS_ASSERT(info.Env(), js_face.IsArray());
      auto&& js_face_arr = js_face.As<JSArray>();

      std::vector<GLfloat> face;
      face.reserve(js_face_arr.Length());

      for(unsigned i = 0; i < js_face_arr.Length(); i++) {
        auto&& js_vertex_value = js_face_arr.Get(i);
        JS_ASSERT(info.Env(), js_vertex_value.IsNumber());

        face.push_back(js_vertex_value.ToNumber().FloatValue());
      }

      auto&& facename = js_facename_str.Utf8Value();
      faces.insert(std::make_pair(facename, std::move(face)));
    }

    auto&& block_definition = std::make_unique<BlockDefinition>(id,
                                                                opaque,
                                                                std::move(faces));

    block_definitions_.push_back(std::move(block_definition));
    return JSNumber::New(info.Env(), block_definitions_.size() - 1);
  }

  void BlockRegistrar::JSCall_loadTexture(JSCbi info) {
    JS_ASSERT(info.Env(), info[0].IsString());

    std::string filepath = info[0].ToString();
    texture_.LoadTexture(filepath.c_str());
  }

  BlockDefinition* BlockRegistrar::GetByIndex(int index) {
    return block_definitions_[index].get();
  }

  const BlockDefinition* BlockRegistrar::GetByIndex(int index) const {
    return block_definitions_[index].get();
  }

  void BlockRegistrar::BindTexture() {
    texture_.ActiveAndBind(GL_TEXTURE0);
  }

} // namespace chunklands::game