#include "BlockRegistrarBase.h"

#include "napi/flow.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(BlockRegistrarBase, ONE_ARG({
    InstanceMethod("addBlock", &BlockRegistrarBase::AddBlock)
  }))

  void BlockRegistrarBase::AddBlock(const Napi::CallbackInfo& info) {
    if (!info[0].IsObject()) {
      THROW_MSG(info.Env(), "expected object as arg");
    }

    auto&& block_definition_napi = info[0].ToObject();

    auto&& id_napi = block_definition_napi.Get("id");
    auto&& vertex_data_napi = block_definition_napi.Get("vertexData");
    auto&& opaque_napi = block_definition_napi.Get("opaque");

    if (!id_napi.IsString()) {
      THROW_MSG(info.Env(), "expected 'id' to be a string");
    }

    if (!vertex_data_napi.IsArray()) {
      THROW_MSG(info.Env(), "expected 'vertexData' to be an array");
    }

    if (!opaque_napi.IsBoolean()) {
      THROW_MSG(info.Env(), "expected 'opaque' to be a boolean");
    }

    auto&& vertex_data_array_napi = vertex_data_napi.As<Napi::Array>();
    std::vector<GLfloat> vertex_data;
    vertex_data.reserve(vertex_data_array_napi.Length());

    for(int i = 0; i < vertex_data_array_napi.Length(); i++) {
      vertex_data.push_back(vertex_data_array_napi.Get(i).ToNumber().FloatValue());
    }

    std::string id = id_napi.ToString();
    bool opaque = opaque_napi.ToBoolean();

    auto&& block_definition = std::make_unique<BlockDefinition>(id,
                                                                opaque,
                                                                std::move(vertex_data));

    block_definitions_.insert(std::make_pair(id, std::move(block_definition)));
  }

  BlockDefinition* BlockRegistrarBase::Find(const std::string& block_id) {
    auto&& it = block_definitions_.find(block_id);
    if (it == block_definitions_.end()) {
      return nullptr;
    }

    return it->second.get();
  }
}
