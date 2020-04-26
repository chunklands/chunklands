#include "BlockRegistrarBase.h"

namespace chunklands {
  JS_DEF_WRAP(BlockRegistrarBase)

  void BlockRegistrarBase::JSCall_addBlock(JSCbi info) {
    auto&& env = info.Env();
    JS_ASSERT(info[0].IsObject());

    auto&& js_block_definition = info[0].ToObject();

    auto&& js_id = js_block_definition.Get("id");
    JS_ASSERT(js_id.IsString());
    std::string id = js_id.ToString();

    auto&& js_opaque = js_block_definition.Get("opaque");
    JS_ASSERT(js_opaque.IsBoolean());
    bool opaque = js_opaque.ToBoolean();

    auto&& js_vertex_data = js_block_definition.Get("vertexData");
    JS_ASSERT(js_vertex_data.IsObject());

    auto&& js_vertex_data_obj = js_vertex_data.As<Napi::Object>();
    auto&& js_facenames_arr = js_vertex_data_obj.GetPropertyNames();
    std::unordered_map<std::string, std::vector<GLfloat>> faces;

    for (unsigned i = 0; i < js_facenames_arr.Length(); i++) {
      auto&& js_facename = js_facenames_arr.Get(i);
      assert(js_facename.IsString());
      
      auto&& js_facename_str = js_facename.ToString();
      auto&& js_face_vertex_data = js_vertex_data_obj.Get(js_facename_str);
      JS_ASSERT(js_face_vertex_data.IsArray());
      auto&& js_face_vertex_data_arr = js_face_vertex_data.As<Napi::Array>();

      std::vector<GLfloat> face_vertex_data;
      face_vertex_data.reserve(js_face_vertex_data_arr.Length());

      for(unsigned i = 0; i < js_face_vertex_data_arr.Length(); i++) {
        auto&& js_face_vertex_value = js_face_vertex_data_arr.Get(i);
        JS_ASSERT(js_face_vertex_value.IsNumber());

        face_vertex_data.push_back(js_face_vertex_value.ToNumber().FloatValue());
      }

      auto&& facename = js_facename_str.Utf8Value();
      faces.insert(std::make_pair(facename, std::move(face_vertex_data)));
    }

    auto&& block_definition = std::make_unique<BlockDefinition>(id,
                                                                opaque,
                                                                std::move(faces));

    block_definitions_.push_back(std::move(block_definition));
  }

  JSValue BlockRegistrarBase::JSCall_getBlockIds(JSCbi info) {
    auto&& js_block_ids = Napi::Object::New(info.Env());

    for (unsigned i = 0; i < block_definitions_.size(); i++) {
      auto&& block_definition = block_definitions_[i];
      js_block_ids.Set(block_definition->GetId(), Napi::Number::New(info.Env(), i));
    }

    return js_block_ids;
  }

  void BlockRegistrarBase::JSCall_loadTexture(JSCbi info) {
    auto&& env = info.Env();
    JS_ASSERT(info[0].IsString());

    std::string filepath = info[0].ToString();
    texture_.LoadTexture(filepath.c_str());
  }

  BlockDefinition* BlockRegistrarBase::GetByIndex(int index) {
    return block_definitions_[index].get();
  }

  void BlockRegistrarBase::BindTexture() {
    texture_.ActiveAndBind(GL_TEXTURE0);
  }
}
