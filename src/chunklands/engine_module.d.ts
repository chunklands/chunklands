import * as gl from './gl_module'

export interface IScene {}
export declare class RenderPass {
  setProgram(program: gl.ProgramBase): void
}

export declare class Environment {
  static initialize(): void
  static loadProcs(): void
  static terminate(): void
}

export declare class GameLoop {
  start(): void
  stop(): void
  loop(): void

  setScene(scene: IScene): void
}

export declare class GBufferPass extends RenderPass {}
export declare class LightingPass extends RenderPass {}
export declare class SkyboxPass extends RenderPass {}
export declare class SSAOBlurPass extends RenderPass {}
export declare class SSAOPass extends RenderPass {}

export interface WindowInitialize {
  width: number
  height: number
  title: string
}

export declare class Window {
  initialize(opts: WindowInitialize): void
  makeContextCurrent(): void
  shouldClose(): boolean
  close(): void
}

export declare class Skybox {
  initialize(texturePath: string): void
}

export declare class Camera {
  setPosition(x: number, y: number, z: number): void
}