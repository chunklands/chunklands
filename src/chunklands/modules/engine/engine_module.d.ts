import * as gl from '../gl/gl_module'
import { EventEmitter } from 'events'

type vec2 = [number, number]
type vec3 = [number, number, number]

export interface IScene {}
export interface ICollisionSystem {}

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

  getGameControl(): boolean
  setGameControl(gameControl: boolean): void

  events: EventEmitter
}

export declare class Skybox {
  initialize(texturePath: string): void
}

export declare class Camera {
  setPosition(x: number, y: number, z: number): void
  getPosition(): vec3
  getLook(): vec2
}

export declare class MovementController {
  setCamera(camera: Camera): void
  setCollisionSystem(collisionSystem: ICollisionSystem): void
}

export declare class FontLoader {
  load(meta: any, imagePath: string): void
}

export declare class TextRenderer extends RenderPass {
  setFontLoader(loader: FontLoader): void
  write(text: string): void
}

export declare class GameOverlayRenderer extends RenderPass {
}