#include "ARenderPass.h"

namespace chunklands {
  void ARenderPass::JSCall_Set_Program(const Napi::CallbackInfo& info) {
    js_Program = info[0];

    js_Program->Use();
    InitializeProgram();
    js_Program->Unuse();
  }
}
