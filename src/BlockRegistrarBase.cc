#include "BlockRegistrarBase.h"

#include "napi/flow.h"
#include "stb.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(BlockRegistrarBase, ONE_ARG({
    InstanceMethod("addBlock", &BlockRegistrarBase::AddBlock),
    InstanceMethod("loadTexture", &BlockRegistrarBase::LoadTexture),
    InstanceMethod("getBlockIds", &BlockRegistrarBase::GetBlockIds),
  }))

  void BlockRegistrarBase::AddBlock(const Napi::CallbackInfo& info) {
    if (!info[0].IsObject()) {
      THROW_MSG(info.Env(), "expected object as arg");
    }

    auto&& js_block_definition = info[0].ToObject();

    auto&& js_id = js_block_definition.Get("id");
    if (!js_id.IsString()) {
      THROW_MSG(info.Env(), "expected 'id' to be a string");
    }
    std::string id = js_id.ToString();

    auto&& js_opaque = js_block_definition.Get("opaque");
    if (!js_opaque.IsBoolean()) {
      THROW_MSG(info.Env(), "expected 'opaque' to be a boolean");
    }
    bool opaque = js_opaque.ToBoolean();

    auto&& js_vertex_data = js_block_definition.Get("vertexData");
    if (!js_vertex_data.IsObject()) {
      THROW_MSG(info.Env(), "expected 'vertexData' to be an object");
    }

    auto&& js_vertex_data_obj = js_vertex_data.As<Napi::Object>();
    auto&& js_facenames_arr = js_vertex_data_obj.GetPropertyNames();
    std::unordered_map<std::string, std::vector<GLfloat>> faces;

    for (int i = 0; i < js_facenames_arr.Length(); i++) {
      auto&& js_facename = js_facenames_arr.Get(i);
      assert(js_facename.IsString());
      
      auto&& js_facename_str = js_facename.ToString();
      auto&& js_face_vertex_data = js_vertex_data_obj.Get(js_facename_str);
      if (!js_face_vertex_data.IsArray()) {
        THROW_MSG(info.Env(), "expected face vertex data to be an array");
      }
      auto&& js_face_vertex_data_arr = js_face_vertex_data.As<Napi::Array>();

      std::vector<GLfloat> face_vertex_data;
      face_vertex_data.reserve(js_face_vertex_data_arr.Length());

      for(int i = 0; i < js_face_vertex_data_arr.Length(); i++) {
        auto&& js_face_vertex_value = js_face_vertex_data_arr.Get(i);
        if (!js_face_vertex_value.IsNumber()) {
          THROW_MSG(info.Env(), "expected vertex data to be a number");
        }

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

  Napi::Value BlockRegistrarBase::GetBlockIds(const Napi::CallbackInfo& info) {
    auto&& js_block_ids = Napi::Object::New(info.Env());

    for (int i = 0; i < block_definitions_.size(); i++) {
      auto&& block_definition = block_definitions_[i];
      js_block_ids.Set(block_definition->GetId(), Napi::Number::New(info.Env(), i));
    }

    return js_block_ids;
  }

  void BlockRegistrarBase::LoadTexture(const Napi::CallbackInfo& info) {
    if (!info[0].IsString()) {
      THROW_MSG(info.Env(), "expected string as arg");
    }

    std::string filePath = info[0].ToString();
    int width, height, comp;
    auto&& data = stbi_load(filePath.c_str(), &width, &height, &comp, 0);
    if (!data) {
      THROW_MSG(info.Env(), "could not load texture");
    }

    GLenum format;
    switch (comp) {
    case 3: {
      format = GL_RGB;
      break;
    }
    case 4: {
      format = GL_RGBA;
      break;
    }
    default: {
      THROW_MSG(info.Env(), "only support 3 or 4 channels");
    }
    }

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);
  }

  BlockDefinition* BlockRegistrarBase::GetByIndex(int index) {
    return block_definitions_[index].get();
  }

  void BlockRegistrarBase::BindTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
  }
}
