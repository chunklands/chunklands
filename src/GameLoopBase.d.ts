import {SceneBase} from './SceneBase'
export {GameLoopBase}

declare class GameLoopBase {
  start(): void
  stop(): void
  loop(): void

  setScene(scene: SceneBase): void
}
