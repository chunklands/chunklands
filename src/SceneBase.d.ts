import { WindowBase } from './WindowBase'
import { WorldBase } from './WorldBase'
import { GBufferPassBase } from './GBufferPassBase'
export {SceneBase}

declare class SceneBase {
  setWindow(window: WindowBase): void
  setWorld(world: WorldBase): void

  setGBufferPass(pass: GBufferPassBase): void
}
