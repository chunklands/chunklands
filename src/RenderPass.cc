#include "RenderPass.h"

namespace chunklands {
  void RenderPass::SetProgram(const Napi::Value& program) {
    program_ = program.ToObject();

    program_->Use();
    InitializeProgram();
    program_->Unuse();
  }

  void RenderPass::Begin() {
    program_->Use();
  }

  void RenderPass::End() {
    program_->Unuse();
  }
}
