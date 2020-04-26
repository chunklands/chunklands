import { WindowBase } from './WindowBase'
import { WorldBase } from './WorldBase'
import { GBufferPassBase } from './GBufferPassBase'
import { SSAOPassBase } from './SSAOPassBase'
import { SSAOBlurPassBase } from './SSAOBlurPassBase'
export {SceneBase}

declare class SceneBase {
  setWindow(window: WindowBase): void
  setWorld(world: WorldBase): void

  setGBufferPass(pass: GBufferPassBase): void
  setSSAOPass(pass: SSAOPassBase): void
  setSSAOBlurPass(pass: SSAOBlurPassBase): void
}
